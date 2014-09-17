;(function(app, Backbone){
	app.ConsoleLine = Backbone.GSModel.extend({
		defaults : {
			body : "",
			emitter : "console",			
		},
		prefix : "> ",
		setters : {
			body : function (body) {	
			console.log(this.prefix + body)			
				return this.prefix + body;
			}			
		},
		raw_body : function() {
			return this.get("body").slice(this.prefix.length);
		}
	})
	app.ConsoleLineCollection = Backbone.Collection.extend({	
		model : app.ConsoleLine,
	});
	app.ConsoleLineView = Backbone.View.extend({
		el : ".console #input div",
		history_line : 0,
		history_length : 0 ,
		initialize : function(options){
			var self = this;
			_.extend(this, options);
			this.cmView = new app.CodeMirror({
				lineNumbers : false,
			});
			this.editor = this.cmView.render(this.$el);			
		
			this.editor.codemirror.on("inputRead", function(){
				self.input_read.call(self);
			});
			this.editor.codemirror.on("keyHandled", function(a,b,c){
				self.key_handled.call(self, a,b,c);
			});
		
			this.context = {};
			app.interpreter.ctx_setup(this.context, "console", ["api", "logger", 
					{ as : "r", lib : "console_api.r"},
					{ as : "m", lib : "console_api.m"}, 
					{ lib : "logger.info", as : "info"},
					{ lib : "logger.error", as : "error"}, ]);
			this.context.eval_str.start += " return ";
			console.dir(this.context);
		}, 

		input_read : function(){
			this.history_line = -1;
		},
		key_handled : function(cm, name, ev){
	
			var line, text;

			switch(name) {
				case "Up" :
				
					if(this.history_line < this.history.length) {
						this.history_line ++;
					
						line = this.history.at( this.history.length  - this.history_line -1);
	
						text = line.raw_body();
						
						this.editor.codemirror.setValue(text);
					}
					break;
				case "Down" :
					if(this.history_line > 0 ) {
						this.history_line --;
					
						line = this.history.at(this.history.length - this.history_line -1);

						text = line.raw_body();

						this.editor.codemirror.setValue(text);
						break;

					}
					this.history_line = -1;				
					this.editor.codemirror.setValue('');
					break;
				case "Enter" :
					line = this.editor.codemirror.getValue();
					this.editor.codemirror.setValue('');
					line = line.replace(/\n/g, '');
					line = line.replace(/ /g, '');
					line = line.replace(/\t/g, '');  // TODO in strings ??
				
					if(line != "" ) {					

						console.dir(this.context);
						app.interpreter.execute(line, this.context);
						this.history.add(new app.ConsoleLine({body :  line}));					
					}
					/* Fall throught */
				default:
					this.history_line = -1;
			}
		},
	})
	app.ConsoleView = Backbone.View.extend({
		
		initialize : function(options){
			
			
			this.history = new app.ConsoleLineCollection();
			this.input = new app.ConsoleLineView({
				history : this.history,
			});
			


			//this.textView = new app.ReadOnlyTextView({
			//	collection : this.history,
			//	filed : "text",
			//});
		}, 
	})
})(app, Backbone);
