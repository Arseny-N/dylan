;(function(app, Backbone){

	app.LoggerEntry = Backbone.GSModel.extend({
		defaults : {
			emitter : "",
			context : "",			
			body 	: "",						
			prio 	: "info"
		},
		getters : {
			line : function () {
				return this.emitter + this.file + this.prio;
			}
		},
		setters : {
			line : function (data) {
				data = data.split(' ');
				this.emitter = data[0];
				this.file = data[1];
				this.prio = data[2];
			}
		},
		prios: ["info", 
			"warn",
			"error"],

	
	});
	
	app.LoggerRawCollection = Backbone.Collection.extend({
		model : app.LoggerEntry,
		initialize: function (options) {
			var self = this;
			_.extend(this, {max_lenght : 50} )
			_.extend(this, options);
			this.on("add", function(){
				if(self.length > self.max_lenght) {
					var m = self.shift();
					m.get("view").$el.remove();
				}
			});		
		}
	});

	app.LoggerScrFilteredCollection = Backbone.Collection.extend({
		model : app.LoggerEntry,
		filed :  ["emitter"],
		regexp : [undefined],
	
		predefined_filter : false, 
	
		initialize: function (models, options) {
			var self = this;
		
			_.extend(this, options);
				
		  	this.start_listen();
		  	
		   	this.listenTo(this.logs.raw, 'add', this.add_one);
			this.listenTo(this.logs.raw, 'reset', this.add_all); 
		},
	
		log_filter: function(le, filed, regexp) {

			return this.regexp ? !!le.get(filed).match(regexp) : true;
		},
		add_one: function( le ) {	
			for(var i in this.filed) {
				var filed  = this.filed[i];
				var regexp = this.regexp[i];
				
				if(this.log_filter(le, filed, regexp)) {
					this.add(le);
					return;
				}
			}
			
	    	},
	    	add_all: function() {
	      		this.reset([], {silent: true});
	      		app.logs.raw.each(this.add_one, this);
	      		this.trigger("reset");
	    	},
	    	stop_listen : function () {
	    		if(!this.predefined_filter)
			    	this.stopListening(app.dispatcher, "changed:regexp");	
	    	},
	   	start_listen : function () {
	   		var self = this;   
			if(!this.predefined_filter) {
				this.listenTo(app.dispatcher, "changed:regexp", function(o){
					var index = _.indexOf(self.filed, o.filed);
					console.log("----->" + index);
					self.regexp[index] = o.regexp;
					
					this.add_all();			
				});	
				app.dispatcher.trigger("disable:regexp", false);
			} else {
				app.dispatcher.trigger("disable:regexp", true);
			}
	
		
	   	}
	})
	app.LoggerMcFilteredCollection = app.LoggerScrFilteredCollection.extend({
		model : app.LoggerEntry,
		filed :  ["emitter"],
		regexp : [ /^script/ ],	
		predefined_filter : true,
	});
	app.LoggerConsFilteredCollection = app.LoggerScrFilteredCollection.extend({
		model : app.LoggerEntry,
		filed : ["emitter", "prio", "context"],
		regexp : [ /^console/, /^error/, /^console/ ],	
		predefined_filter : true,
		
		initialize: function (models, options) {
			var self = this;
		
			_.extend(this, options);
			app.LoggerConsFilteredCollection.__super__.initialize.apply(this);
			
			this.console = app.console;
			
		   	this.listenTo(this.console.history, 'add', this.add_one);
			this.listenTo(this.console.history, 'reset', this.add_all); 			
		},
	});



	/*--------------------------------------------------------*/


	app.LoggerScrEntryView = Backbone.View.extend({
		model : app.LoggerEntry,
		tagName : "tr",
		prioToClass : {	
			"debug": "",
			"info" : "",
			"warn" : "warning",
			"error": "danger",
		},
		template_selector : "script.template.logger-src-entry",
		initialize: function (options) {
			this.template = app.utils.mk_templ(this.template_selector);

			this.$el.addClass(this.prioToClass[this.model.get("prio")]);
			this.model.set("view", this);
		},
		render: function() {		
	    		this.$el.html( this.template(this.model.toJSON()) );   
	    		return this;
	  	}	
	});
	app.LoggerMcEntryView = app.LoggerScrEntryView.extend({
		template_selector : "script.template.logger-mc-entry"	
	});
	app.LoggerConsEntryView = app.LoggerScrEntryView.extend({
		template_selector : "script.template.logger-console-entry"	
	});
	/*--------------------------------------------------------*/
	app.RegExpInputView = Backbone.View.extend({
		el : "div#logger-regexp ",
		text : "",
		regexp : undefined,
		events : {
			"input input": "change_regexp"	
		},
		initialize: function (options) {
			var self = this;
			this.$input = this.$("input");
			this.$input.val('');
			this.listenTo(app.dispatcher, "disable:regexp", function(on){
				self.$input.prop({disabled : on});
			});	
		},	
		change_regexp : function(evt) {
			evt.preventDefault();
			this.text = this.$input.val();
			this.regexp = new RegExp(this.text);
			if(this.text == "")
				this.regexp = undefined;
		
			app.dispatcher.trigger("changed:regexp", { regexp : this.regexp, filed : "emitter" });
		},
	
	
	})
	/*--------------------------------------------------------*/

	app.LoggerToolbarView = Backbone.View.extend({
		el : "#logger-toolbar",
		events : {		
			"click #copy-to-clbd" : "copy_to_clib_board",						
			"click #clear" : "clear",
		},
		initialize: function (options) {
			this.options = options || {};						

		},
		copy_to_clib_board : function () {	
			var logs = app.logs.filtered.toJSON();
		
			var res = _.reduce(logs, function (memo, log) {
				return memo + log.emitter + " " + log.prio + " " + log.body + "\n"
			}, "");
			console.dir(res);
		
			app.utils.copyToClipboard(res);
		
		},	
		clear : function () {	
			app.logs.raw.reset();
		}				
	});

	app.LoggerTabs = Backbone.View.extend({
		el : "#logger-tabs",
		events : {		
			"click #script-tab" : function(){
				this.logger.set_mode("script");
			},						
			"click #chip-tab" : function(){
				this.logger.set_mode("mc");
			},
			"click #console-tab" : function(){
				this.logger.set_mode("console");
			},
					
		},
		initialize: function (options) {
			_.extend(this, options);					
		},	
		
	}); 


	app.Logger = Backbone.View.extend({
		el : "#logger",
		modes : {
			"script" : {
				View : app.LoggerScrEntryView,
				Collection : app.LoggerScrFilteredCollection				
			},
			"mc" : {
				View :app.LoggerMcEntryView,
				Collection : app.LoggerMcFilteredCollection				
			},
			"console" : {
				View :app.LoggerConsEntryView,
				Collection : app.LoggerConsFilteredCollection,
				init : function(){
					this.$consoleDiv = this.$("div.console");
				},
				start : function () {
					this.$consoleDiv.removeClass("hidden");
				},		
				stop : function () {
					this.$consoleDiv.addClass("hidden");
				},
			}
		},

	
		initialize: function (options) {
			_.extend(this, options);
		
			this.$table = this.$("#logger-container table");
			
			this.regexp_input = new app.RegExpInputView({});
			this.tabs = new app.LoggerTabs({
				logger : this
			});
			this.toolbar = new app.LoggerToolbarView({});
			this.raw = new app.LoggerRawCollection();
			
			var $cont = $('#logger-container');
			this.raw.on('add', function(){
				$cont.get(0).scrollTop=$cont.get(0).scrollHeight;
			})
			
			this.set_mode("mc");
			this.set_mode("script");								
		},
		start_listen : function(o) {
			if(o) {
			  	this.listenTo(o, 'add', this.add_one);
				this.listenTo(o, 'reset', this.add_all);
				this.listenTo(o, 'clear', this.clear);
				
				if(o.start_listen)
					o.start_listen();
			}
		},
		stop_listen : function(o) {
			if(o) {
				this.stopListening(o);
				if(o.stop_listen)
					o.stop_listen();
			}
		},
		set_mode : function (mode_name){
			var mode = this.modes[mode_name];

			if(!mode) 
				return;
			
			if(this.stop)
				this.stop.apply(this);
				
			this.stop_listen(this.filtered);
			
			if(!mode.cache || !mode.cache.collection ) {
				if(!mode.cache) 	   mode.cache = { };
				if(!mode.cache.collection) mode.cache.collection = new mode.Collection([],{logs : this});
			}
			
			this.filtered = mode.cache.collection;
			this.start_listen(this.filtered);
			if(mode.init) {
				mode.init.apply(this);
				mode.init = undefined;
			}
			
			if(mode.start)
				mode.start.apply(this);
				
			this.stop = mode.stop;			
			this.View = mode.View;							
			this.add_all();
		},
		
		add_one: function( le ) {
				
	      		var entry = new this.View({ model: le });
		    	this.$table.append( entry.render().el );	    	
		    	
	    	},
	   	add_all: function() {
	   		this.clear();
	      		this.filtered.each(this.add_one, this);
	    	},    
	    	clear: function() {
	    		this.$table.html('');
	    	}	    
	})
	
	/*--------------------------------------------------------*/

	app.log = function( emitter,file, body, prio) {
		var r;
		prio = prio || "info";
		
		if(typeof emitter == "object")  r = emitter;
		else r = { emitter : emitter, body : body, prio : prio, context : file};
		app.logs.raw.add(new app.LoggerEntry(r));
	}
	
})(app, Backbone); 
