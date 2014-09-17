;var app = app || {};

;(function(app, Backbone, CodeMirror){

	Backbone.GSModel = Backbone.Model.extend({
		get: function(attr) {
		// Call the getter if available
			if (_.isFunction(this.getters[attr])) {
				return this.getters[attr].call(this);
			}

			return Backbone.Model.prototype.get.call(this, attr);
		},

		set: function(key, value, options) {
			var attrs, attr;

			// Normalize the key-value into an object
			if (_.isObject(key) || key == null) {
				attrs = key;
				options = value;
			} else {
				attrs = {};
				attrs[key] = value;
			}

			// always pass an options hash around. This allows modifying
			// the options inside the setter
			options = options || {};

			// Go over all the set attributes and call the setter if available
			for (attr in attrs) {
				if (_.isFunction(this.setters[attr])) {
					attrs[attr] = this.setters[attr].call(this, attrs[attr], options);
				}
			}

			return Backbone.Model.prototype.set.call(this, attrs, options);
		},

		getters: {},

		setters: {}

	});

	app.Document = Backbone.GSModel.extend({
		defaults : {
			type : "none",
		
			readOnly : false,
		
			name : "",
			data : ""
		
		},
		disk_read_setup : function (fn) {
			var self = this;
			this.reader = new FileReader();
			this.reader.onload = function (e) {	
				if (e.target.readyState == FileReader.DONE)		
					self.set("data", e.target.result);
				return fn(e);
			}
			return this.reader;
		}	
	})
	app.DocumentText = app.Document.extend({
		defaults : {
			type : "text",		
			encoding : ""
		
		},
		mode : {name: "text" },
	
		disk_read : function(f, fn) {
			return this.disk_read_setup(fn).readAsText(f, this.get("encoding"));		
		},	
		getters : {
			data : function() {
				if(this.CodeMirrorDoc)
					return this.CodeMirrorDoc.getValue();
				return "";
			},
			CodeMirrorDoc : function(){
				if(!this.CodeMirrorDoc)				
					this.CodeMirrorDoc = new CodeMirror.Doc("", this.mode);
				return this.CodeMirrorDoc;
			}
		},
		setters : {
			data : function(data) {
				if(!this.CodeMirrorDoc)				
					this.CodeMirrorDoc = new CodeMirror.Doc("", this.mode);				
				return this.CodeMirrorDoc.setValue(data);				
			}
		},
		
	})
	app.DocumentScript = app.DocumentText.extend({
		defaults : {		
			type : "script",
			encoding : ""
		
		},
		mode : {name: "javascript", json: true}
	})
	app.DocumentBin = app.Document.extend({
		defaults : {
			type : "binary",		
			readOnly : true
		},
		disk_read : function(f, fn) {
			return this.disk_read_setup(fn).readAsBinaryString(f);		
		}
	})


	app.DocumentCollection = Backbone.Collection.extend({	
		model : app.Document,	
		url : "/api/documents",
		localStorage: new Backbone.LocalStorage("dylan-documents2", {
			serialize : function(model){
				var obj = {	
					name : model.get("name"),
					data : model.get("data"),
					type : model.get("type"),
				};
				obj[model.idAttribute] = model.get(model.idAttribute);
				
				console.dir(obj);
				return JSON.stringify(obj);
			},
			deserialize : function(s){
				console.log("-------->" + s);
				var obj = JSON.parse(s);
				if(!obj)
					return undefined;
				
				var Model = obj.type == "text" 	 ? app.DocumentText :
					    obj.type == "script" ? app.DocumentScript :  undefined;
				
				if(!Model)
					return undefined;
				var m = new Model(obj, {collection : app.docs});	
				console.dir(m);
				return m;
			}
		}),
		
		initialize : function() {
			var self = this;
			this.on("close:file", this.close_file);
			this.on("save:files", function() {				
				self.each(function(model){
					if(!model.get("sc-id")) {
						model.set("sc-id", Math.round(Math.random(16) * 100000000000).toString(16));
					}
					model.save();
				})							
			});
			
		},
		uniqueName : function(){
			return "random-"+Math.round(Math.random(16) * 100000000000).toString(16);
		},
		close_file : function(doc) {
			console.log("----------------------------");
			console.dir(doc);
			
			doc.destroy();
			this.trigger("change:file", this.at(0));
		}		
	});
	
})(app, Backbone, CodeMirror);
