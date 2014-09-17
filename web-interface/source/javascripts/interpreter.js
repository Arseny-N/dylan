;(function(app){
	app.Interpreter = function (options) {
		_.extend(this, options);
	};	
	app.Interpreter.prototype.find_runtime_lib = function(name) {
	
			var lib = this.docs.where({name : name });
			if(lib && lib.length)
				return lib[0];
		
			if(name.match(/.js$/)) 
				return undefined;	
			name += ".js";
		
			lib = this.docs.where({name : name });
			if(lib.length)
				return lib[0];
			return undefined;		
	},
	app.Interpreter.prototype.get_runtime_lib = function(name, f) {
			var doc = this.find_runtime_lib(name);
			var rv;
			if(doc) {
				rv = this.execute(doc);
				if(rv.rv) {
					this.log("interpreter:get_runtime_lib",f, "Failed to load "
						+name+", non null return value("+this.pretty_string(rv.rv)+")", "error");
				}
				return rv.exports;
			} 
			return undefined;
	}
	app.Interpreter.prototype.ctx_setup = function( __context__, __filename, req_libs) {
			__context__.self = this;
			__context__.required_libs = {};
		
			__context__.libs = this.libs;
			__context__.libs.logger = app.libs.gen_logger(__filename);
			__context__.libs.documents = app.libs.gen_docs(this.docs);
			__context__.docs = this.docs;
			var sadd = "";
			var eadd = "";


			
			for(var i in req_libs) {
				var lib = req_libs[i];
				var as = lib;
				if(typeof lib == "object") {
					as = lib.as;
					lib = lib.lib;
				}				
				sadd += ", " + as;	
				eadd += ", this.libs."+ lib + "";		
			}
			__context__.eval_str = {};
			__context__.eval_str.start = ";(function ___main___(require, exports, __filename" + sadd+") { \n";
			__context__.eval_str.end =  " \n;})(require, exports, __filename"+eadd+");";
			__context__.eval_str.abort =  ""; //"__abrt_listen__()";
		
		
	}
	app.Interpreter.prototype.execute = function(__document__ , __context__) {				
			var __exeption__ = undefined;
			var __filename = (__document__.get &&  __document__.get("name")) || "console";
			var __filedata = (__document__.get && __document__.get("data") ) || __document__;
		
			this.rv = undefined;
		
			__context__ || this.ctx_setup((__context__ = {}), __filename);

			exports = {};
		
			function require(s) {
				var lib = __context__.libs[s];
			
				if(lib) {
					__context__.required_libs[s]=lib;
					return lib;
				}
				var saved_ex = exports;
				lib = __context__.self.get_runtime_lib(s, __filename);
				exports = saved_ex;
				if(lib) {
					__context__.required_libs[s]=lib;				
					return lib;
				}		
					
				__context__.self.log("interpreter:require", __filename , "Library "+s+" not found.");
				throw {
					name : "NoLibraryFound",
					text : "no library \"" + s + "\" found."
				};
			}	
		
			function required(s, yf, nf) {
				__context__.required_libs && __context__.required_libs[s] && f() || nf();		
			}
			function exit(s) {			
				throw {name : "ExitExeption", status : s};
			}

			function _ex(string) {	
		
				var str = this.eval_str.abort +
				this.eval_str.start + string+
				this.eval_str.end;
				return eval(str);

			}
		
			if(__filename != "console")
				this.log("interpreter:execute", __filename , "Started script \"" + __filename +"\" execution.");
		
			try {
				this.rv = _ex.call(__context__, __filedata );
				console.log(this.rv);
			} catch( __exeption__ ) {
				switch(__exeption__.name) {
					case "ExitExeption": 
						this.rv = __exeption__.status;
						break;
					default :					
						this.log("interpreter:execute", __filename , "Script \"" + __filename  +"\" aborted execution. ", "error");
						this.log("interpreter:execute", __filename , "Exeption: "+this.pretty_string(__exeption__)+".", "error");
						this.log("interpreter:execute", __filename,  "Stack:" + __exeption__.stack)
				}
			}
			if(!__exeption__ && __filename != "console") {
				this.log("interpreter:execute",  __filename , "Script \"" + __filename +"\" exited.");
				this.rv && this.log("interpreter:execute" ,  __filename , "Return value: " + this.pretty_string(this.rv));
				this.log("interpreter:execute" ,  __filename , "Exporting: " + this.pretty_string(exports));
			}
			if(__filename == "console") {
				this.rv && this.log("interpreter:execute" ,  __filename , this.pretty_string(this.rv));
			}
			return { status : this.rv,
				 exports : exports,
			}
	}
	app.Interpreter.prototype.pretty_string = function(o) {
		console.dir(o);
		return app.utils.object2string(o);
	}

})(app);
