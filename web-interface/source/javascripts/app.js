/*--------------------------------------------------------*/
(function (app, CodeMirror, Backbone, _) {
	
	app.CodeMirror = Backbone.View.extend({
		tagName : "div",
		initialize: function (options) {
			this.options = {
				tabSize : 8,
				indentWithTabs : true,
				lineNumbers : true,
				lineWarping : true,
				theme: ["pastel-on-dark", "ambiance", "cobalt" ][1]
			}
			_.extend(this.options, options);
		},


		render : function($el){
			var area = $el.get(0);
//			this.codemirror = CodeMirror(function(elt) {
//	  			area.parentNode.replaceChild(elt, area);
//			}, this.options);
			console.log($el);
			console.log($el[0]);	 		 	
			console.log("==============");
			this.codemirror = CodeMirror(area, this.options);
			return this;		
		}	
	});
	app.ScriptEditor  = Backbone.View.extend({
		model : app.DocumentScript,
		el : "div#cm-script-editor",
		initialize: function (options) {
	
			this.cmView = new app.CodeMirror(options);
			this.codemirror = this.cmView.render(this.$el).codemirror;
		
			this.toolbar = new app.EditorToolbarView({editor : this});
			this.tabs = new app.TabListView();		
	
			this.listenTo(app.docs, "change:file", this.change_doc);
			this.listenTo(app.dispatcher, "run:file", this.run);		

		},
		run : function() {

			ret = app.interpreter.execute(this.model);
			return ret;
		},
		change_doc : function(doc) {
	
			this.model = doc;
			this.codemirror.swapDoc(doc.get("CodeMirrorDoc"));		
		},


	})
	app.ReadOnlyTextView = Backbone.View.extend({	
		el : ".console #view",
		history_line : 0,
		history_length : 0 ,

		initialize : function(options){
		
			this.filed = "text";
			_.extend(this, options);

		
			this.cmView = new app.CodeMirror({
				lineNumbers : false,
				readOnly : true,
			});
			this.editor = this.cmView.render(this.$el);		
		
			this.listenTo(this.collection, 'add', this.add_one);
		}, 
		add_one : function(line) {
			var val = this.editor.codemirror.getValue();	
			val += line.get(this.filed) + "\n";
			this.editor.codemirror.setValue(val);	
		}
	
	})
	app.EditorToolbarView = Backbone.View.extend({
		el : "div#editor-toolbar",

		events : {
			"click #close-doc" : function(){
				app.dispatcher.trigger("close:file");
			},
			"click #copy-to-clbd" : "copy_to_clib_board",
			"click #add-doc-empty" : "add_doc",
			"click #add-text-doc-empty" : "add_text_doc",
			
			"click #add-doc-disk" : "add_doc_disk",
			
			"click #run-doc" : "run_doc",
			"click #download" : "download",

			"click #console-toggle" : "console_toggle",
		
			"click #rename-doc" : "rename_doc",
			"click #save" : function(){
				app.docs.trigger("save:files");
			},
		

		
			"click #add-disk-script" : function(){ app.utils.files.load(app.DocumentScript); },
			"click #add-disk-text" :   function(){ app.utils.files.load(app.DocumentText); },
		},
		initialize: function (options) {
		
			_.extend(this, {
				editor : undefined,
			})
			_.extend(this, options);
		
			this.$sEditor = this.$("#script-editor");
			this.$console = this.$(".console");
		
			this.$console_btn = this.$("#console-toggle span.cns-content");
			this.$console_icon = this.$("#console-toggle span.glyphicon");
		

		
		},	
	
		

		rename_doc : function(evt){

			app.dispatcher.trigger('name-change');
		
		},
		copy_to_clib_board : function () {	 
			app.utils.copyToClipboard(this.editor.model.get("data"));
		},
		add_doc : function () {	
			var doc = new app.DocumentScript({name:app.docs.uniqueName() + ".js", data: ""});
			app.docs.add(doc);
			app.docs.trigger("change:file",doc); 
		},

		add_text_doc : function () {	
			var doc = new app.DocumentText({name:app.docs.uniqueName() + ".txt", data: ""});
			app.docs.add(doc);
			app.docs.trigger("change:file",doc); 
		},
		run_doc : function () {	
			app.dispatcher.trigger('run:file');

		},
		download : function() {
			var doc = this.editor.model;
			app.utils.files.save(doc);
		},
			
	
	});

	/*-------------------------------------------------*/
	app.TabView = Backbone.View.extend({
		model : app.Document,	
	
		template : "",
		tagName : "li",
		events : {
			"click .cross" : "close",
		},
	
		initialize: function (options) {
			var type = this.model.get("type");
		
			this.template = app.utils.mk_templ("script.template.doctab."+type);
		
	
		
		},
		render: function() {	
			console.dir(this.model.toJSON());
	    		this.$el.html( this.template(this.model.toJSON()) );    		

			this.$form = this.$("form");
			this.$name = this.$("div.file-name");
			this.$btn = this.$("form button")
			this.$input = this.$("form input")
			console.dir(this);

			var self = this;
			this.$btn.click(function(evt){

				self.model.set("name", self.$input.val());
				app.dispatcher.trigger("name-show");
				app.dispatcher.trigger("reset-tabs");
				evt.preventDefault();									
			});

	    		return this;
	  	},
	  	close : function() {
	  		this.$el.html('');
	  	},
	 
	  	
	});

	app.TabListView = Backbone.View.extend({
		view : app.TabView,
		el : "#editor-tabs",
		active : undefined,
		tabs : [], 
		events : {
			'shown.bs.tab a[data-toggle="tab"]' : "change_active_doc",
		},
		initialize: function (options) {
			this.options = options || {};				

		  	this.listenTo(app.docs, 'add', this.add_one);
			this.listenTo(app.docs, 'reset', this.add_all);
			this.listenTo(app.docs, 'clear', this.clear);
			this.listenTo(app.docs, 'change:file', this.set_active);
		
			this.listenTo(app.dispatcher, 'name-change', this.name_change);
			this.listenTo(app.dispatcher, 'name-show', this.name_show);
			this.listenTo(app.dispatcher, 'reset-tabs', this.add_all);

		},	
		name_change : function() {
			if(this.active) {
				this.active.$form.removeClass("hidden");
				this.active.$name.addClass("hidden");
			}
		},
		name_show : function() {
	
			if(this.active) {
				this.active.$form.addClass("hidden");
				this.active.$name.removeClass("hidden");
			}
		},
		find_view : function( name) {
			return _.find(this.tabs, function(view){
	    			return (view.model.get("name") == name);
	    		})
		},
		add_one: function( file ) {
	      		var view = new this.view({ model: file });
	      		this.tabs.push(view);
		    	this.$el.append( view.render().el );	    	
	    	},
	   	add_all: function() {
	      		this.clear();
	      		app.docs.each(this.add_one, this);
	    	},    
	    	clear: function() {
	    		this.tabs = [];
	    		this.$el.html('');
	    	},
	    	set_active: function(doc) {
			this.name_show();
	    		this.active = this.find_view(doc.get("name"));
	    		this.active.$el.tab("show") ;    		    		
		},
		close_active : function(doc){
			this.name_show();
			this.active.close();
			for(var i in this.tabs) {
				if(this.tabs[i].model.get("name") == this.active.model.get("name")) {
					console.dir(this.tabs[i]);
					delete this.tabs[i];
				}
			}
		},
	    	change_active_doc : function(e) {    		
	    		var name = $(e.target).attr("name-attribute");
		    	var doc = app.docs.findWhere({name:name});

	    		app.docs.trigger("change:file",doc);    		    				
	    	}
	});


	/*--------------------------------------------------------*/
	app.Dialog =  Backbone.Model.extend({
		defaults : {
			modal_options : {			
			},
			title : "",
			body : "",
		},
	})
	app.DialogView =  Backbone.View.extend({
		model : app.Dialog,
		initialize: function (options) {
			this.template = app.utils.mk_templ("script.template.doctab");		
		},
		render: function() {		
	    		this.$el.html( this.template(this.model.toJSON()) );    		

	    		return this;
	  	},
		show : function () {
			this.$el.modal("show");
		},
		hide : function () {
			this.$el.modal("hide");
		}
	})

	/*--------------------------------------------------------*/
	app.SrcEditor = Backbone.View.extend({
		el: "section#editor",

		initialize: function (options) {
			this.options = options || {};				
			this.activeIndex = 0;
			this.docs = app.docs;
			this.sh_cut = false;
			var self = this;
			this.listenTo(app.docs, "change:file", function (e) {
				if(!this.sh_cut) {
					var ids = app.docs.pluck("name");
					var id = _.indexOf(ids, e.get("name"));
					this.activeIndex = id;
				}
			});
			this.listenTo(app.dispatcher, "close:file",this.close_doc);			
			this.$cheatsheet = $("#cheatsheet");
			
			Mousetrap.bindGlobal('ctrl+s', function(e) {
				e.preventDefault();
				app.docs.trigger("save:files");
    				console.log("SAVED");
			});
			Mousetrap.bindGlobal('alt+r', function(e) {
				e.preventDefault();
				app.dispatcher.trigger('run:file');
    				console.log("SAVED");
			});
			Mousetrap.bindGlobal('ctrl+alt+pagedown', function(e) {
				e.preventDefault();
				self.sh_cut = true;
				self.activeIndex ++;
				if(self.activeIndex >= app.docs.length)
					self.activeIndex =  0;

				doc = app.docs.at(self.activeIndex);					
				app.docs.trigger("change:file", doc);
			});
			Mousetrap.bindGlobal('alt+h', function(e) {
				e.preventDefault();
				console.log("HELLO");
				console.dir(self.$cheatsheet);
				self.$cheatsheet.dropdown('toggle');
			});
		 	Mousetrap.bindGlobal('ctrl+k', function(e) {
			 	e.preventDefault();
		 		var doc = new app.DocumentScript({name:app.docs.uniqueName() + ".js", data: ""});
				app.docs.add(doc);
				app.docs.trigger("change:file",doc); 
		 	});
		 	Mousetrap.bindGlobal('ctrl+l', function(e) {
			 	e.preventDefault();
		 		var doc = new app.DocumentText({name:app.docs.uniqueName() + ".txt", data: ""});
				app.docs.add(doc);
				app.docs.trigger("change:file",doc); 
		 	});
			
			Mousetrap.bindGlobal('ctrl+o', function(e) {
			e.preventDefault();
				app.utils.files.load(app.DocumentScript)
		 	});
		 	Mousetrap.bindGlobal('ctrl+p', function(e) {
		 	e.preventDefault();
				app.utils.files.load(app.DocumentText)
		 	});

			
			
		 	Mousetrap.bindGlobal('ctrl+alt+pageup', function(e) {
				e.preventDefault();
				self.sh_cut = true;
				self.activeIndex --;
				if(self.activeIndex < 0) 					
					self.activeIndex = app.docs.length - 1;
				
				
				var doc = app.docs.at(self.activeIndex);

				app.docs.trigger("change:file", doc);
			});
			var genBind = function(num) {
				Mousetrap.bindGlobal("alt+"+num.toString(), function(e) {
					e.preventDefault();
					if(num-1 < app.docs.length) {					
						self.sh_cut = true;
						self.activeIndex  = num-1;						
						var doc = app.docs.at(num-1);
						app.docs.trigger("change:file", doc);
					}
				});
			}
			for(var i = 0; i< 10; ++i)
				genBind(i+1);
				
			Mousetrap.bindGlobal('alt+w', function(e) {
				e.preventDefault();
				self.sh_cut = true;
				self.activeIndex = 0;
				app.dispatcher.trigger("close:file");
			});
			this.editor = new app.ScriptEditor();
			
			
			var $doc = $(document);			
			var percent = 70;
			$(window).on("resize", function(){	
				var height = $.el_height($doc.get(0));
				self.editor.codemirror.setSize(undefined, (height/100 * percent ) - 150 );				
			}).trigger("resize");

			

		},
		close_doc : function () {	
			if(app.docs.length > 1) {			
				this.editor.tabs.close_active();
				app.docs.close_file(this.editor.model);
			}

		},
	})
	/*--------------------------------------------------------*/

	/*app.Router =  Backbone.Router.extend({
		routes :{
			help : "help",
			editor : "editor",
			memory : "memory",
			registers : "registers",
		},

		initialize: function (options) {
			this.$current = $("section#editor");
			console.log("=====================");
			console.dir(this.$current);
		
			this.on("route:help", app.utils.gen_route("section#help"))
			this.on("route:editor", app.utils.gen_route("section#editor"))
			this.on("route:memory", app.utils.gen_route("section#mem"))
			this.on("route:registers", app.utils.gen_route("section#regs"));
		},			
	});
	*.

	/*--------------------------------------------------------*/
	app.AppView = Backbone.View.extend({
		def_doc_list : [new app.DocumentScript({name:"script.js", data: "/* Hello !*/"})],
		initialize: function (options) {
			app.dispatcher = _.clone(Backbone.Events);
			app.docs = new app.DocumentCollection([]);
		
		

				
			app.utils.initialize();		
			app.interpreter = new app.Interpreter({
				docs : app.docs,
				log  : app.log, 
				libs : app.libs,
			})
			app.editor = new app.SrcEditor({});
			app.logs = new app.Logger();
			app.console = new app.ConsoleView();		

			app.docs.fetch();

			if(!app.docs.at(0)) 				
				app.docs.add(this.def_doc_list);
			

			app.docs.trigger("change:file",app.docs.at(0));  				
			
			// app.router = new app.Router();
			
			// Backbone.history.start(); 						
		}
	})

})(app, CodeMirror, Backbone, _);

