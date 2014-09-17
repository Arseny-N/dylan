;(function(app, _){
	app.libs = {}
	app.libs.gen_logger = function(fname){
		return {
			error : function(msg) {
				if(typeof msg != 'string')
					msg = msg.toSource ? msg.toSource() : msg.toString();
				app.logs.raw.add(new app.LoggerEntry({ emitter: "script", context : fname,body : msg, prio : "danger"}));
			},
			info : function(msg) {
				if(typeof msg != 'string')
					msg = msg.toSource ? msg.toSource() : msg.toString();
			
				app.logs.raw.add(new app.LoggerEntry({ emitter: "script", context : fname,body : msg, prio : "info"}));
			},
			warn : function(msg) {
				if(typeof msg != 'string')
					msg = msg.toSource ? msg.toSource() : msg.toString();
				app.logs.raw.add(new app.LoggerEntry({ emitter: "script", context : fname,body : msg, prio : "warning"}));
			},
			dir : function(o, opts) {
				return app.utils.object2string(o, opts);
			}
		}
	}
	app.libs.gen_docs = function(docs) {
		return {
			list : function() {
				return docs.map(function(doc){return doc.get("name")});
			},	
			get_data  : function(name) {						
				var r = docs.where({name : name})[0];			
				return r ? r.get("data") : r;
			},
			set_data  : function(name, data) {						
				var r = docs.where({name : name})[0];			
				return r ? r.set("data", data) : r;
			}		
		
		}
	}

	app.libs._ = app.libs.underscore = _;
	app.libs.api = {
		reg : {},
		mem : {}
	};


	function __do_request(req, fname, fn) {
		function parse_xhr(xhr) {			
			console.dir(xhr);
			var e, o;
			try {
				o = JSON.parse(xhr.responseText).result
			} catch(e) {
				var err = {
					text : "Broken response from server",
					data : xhr.responseText,
					name : "FailedResponseExeption",
				}
				throw err;
			}
			return o;
		}
		function complete(res, status) {
			app.utils.currrentOp("");		

			switch(status) {
				case "success":					
					return fn && fn(undefined, parse_xhr(res));
				default :

					var err = {
						code : res.status,
						text : res.statusText,
						data : res.responseText,
					}
					if(!fn) {
						err.name = "FailedResponseExeption"
						throw err;
					}		
					return fn(err);
			}
		}
		var xhr = app.api[fname]({req : req}).complete(complete);
		
		
		if(xhr.status == 200)			
			return parse_xhr(xhr);
			
		var err = {
			code : xhr.status,
			text : xhr.statusText,
			data : xhr.responseText,
		}

		return err;		
		
	}
	app.libs.api.mem.write = function(address, data, fn) {
		var req = [ address, data ];
		
		if(typeof address != 'number' ) 
			throw TypeError("`address' and `count' should be a number");		
		if(data.__proto__ != Array.prototype ) 
			throw TypeError("`data' should be an array of numbers");		
		for(var i=0; i< data.length; ++i)
			if(typeof data[i] != 'number')
				throw TypeError("`data' should be an array of numbers");
	

		app.utils.currrentOp("mem.write");
		return __do_request(req, "mem_write", fn);
	}	

	app.libs.api.mem.read = function(address, count, fn) {
		var req = [ address, count ];
	
		if(typeof address != 'number' || typeof count != 'number') 
			throw TypeError("`address' and `count' should be a number");
		app.utils.currrentOp("mem.read");
		return __do_request(req, "mem_read", fn);
	}

	app.libs.api.reg.read = function(reg, fn) {		
		var req = [ reg	];

		if(typeof reg != 'number' ) 
			throw TypeError("`reg' should be a number");
		app.utils.currrentOp("reg.read");
		return __do_request(req, "reg_read", fn);
	}

	app.libs.api.reg.write = function(reg, data, fn) {			
		var req = [ reg, data ];
	
		if(typeof reg != 'number' ||  typeof data != 'number') 
			throw TypeError("`data' and `reg' should be a number");
	
	
		app.utils.currrentOp("reg.write");
		return __do_request(req, "reg_write", fn);
	}	

	app.libs.api.poll = function(address, mask, value, ntimes) {
		ntimes || (ntimes = 50);
		
		if(typeof address != 'number'|| 
		   typeof mask 	  != 'number'|| 
		   typeof value   != 'number') 
			throw TypeError("`address', `mask', `value' should be a number");
		
		app.utils.currrentOp("poll");
		var rval;
		do {
			rval = app.libs.api.reg.read(address);
			ntimes -= 1;
		} while(((rval & mask) != (value & mask)) && (ntimes > 0) );
		app.utils.currrentOp("");		
		return;
	}
	app.libs.api.wait = function(address, mask, value, fn, delay, ntimes) {
		delay || (delay = 100);
		ntimes || (ntimes = 50);
		
		if(typeof address != 'number' || 
		   typeof mask    != 'number' || 
		   typeof value   != 'number' ||
		   typeof ntimes  != 'number') 
			throw TypeError("`address', `mask', `value' should be a number");
			
		setTimeout(function again(){
			app.utils.currrentOp("wait");
			var rval = app.libs.api.reg.read(address);
			ntimes -= 1;
			if((rval & mask) == (value & mask) || ntimes == 0) {
				fn && fn(rval);				
				return;
			}
			app.utils.currrentOp("");
			setTimeout(again, delay);
		}, delay)
		return;
	}

	app.libs.console_api = { r : {}, m : {}};
	app.libs.console_api.m.w = function(addr, data) { 
		return app.libs.api.mem.write(addr, data);		
	}
	
	app.libs.console_api.m.r = function(addr, data) { 
		return app.libs.api.mem.read(addr, data);
	}

	app.libs.console_api.r.r = function(addr) { 
		return app.libs.api.reg.read(addr); 
	}
	app.libs.console_api.r.w = function(addr, data) { 
		return app.libs.api.reg.write(addr, data);
	}
	
})(app, _);
