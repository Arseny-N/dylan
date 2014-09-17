
/**
 * Module dependencies.
 */

var express = require('express');
var http = require('http');
var path = require('path');

function is_present(o, str) {
	for(var i in o) {
		if(	o.hasOwnProperty(i) &&
			 i == str)
			return i;			
	}
	return undefined;	
}

var ps = {};
(function(root) {
	root.Emulator = function(options) {
		this.regs = {
			"eax" : 0xffff,
			"ebx" : 0xff4f,
			"ecx" : 0xffff,
			"edx" : 0xfffff,
			"1" : 0xf,
		};
		this.mem = [];

		this.mem[0] = 0;
		this.mem[256] = 0;
		for(i in options) 
			this[i] = options[i];	
	};
	root.Emulator.prototype.data_dump = function(req) {		
		console.log("======== registers ========");
		console.dir(this.regs);
		console.log("======== memory ========");
		console.dir(this.mem);		
		
		return { result : {
				mem : this.mem,
				regs : this.regs,
		} };
	},
	root.Emulator.prototype.read_reg = function(req) {		
		var ind = is_present(this.regs, req[0]);
		if(ind) 
			reg = this.regs[ind];
		else 	
			reg = 0;				
		return { result : reg };
	};
	root.Emulator.prototype.write_reg = function(req) {
		var ind = is_present(this.regs,req[0]);
		if(ind) 
			this.regs[ind] = req[1];					
		return { result : "success" };
	};
	root.Emulator.prototype.write_mem = function(req) {			
		var address = req[0];
		var data = req[1];
		for(var i=0; i < data.length; ++i) {	
			if(this.mem.length <= address + i ) 
				break;
			
			this.mem[address + i] = data[i];			
		}						
		return { result : "success" };
	};
	root.Emulator.prototype.read_mem = function(req) {			
		var address = req[0];
		var count = req[1];
		var data = [];
		
		for(var i=0; i < count; ++i) {	
			if(this.mem.length <= address + i ) 
				break;
			
			data[i] = this.mem[address + i];
		}			
					
		return { result : data };
	}
})(ps);

var em = new ps.Emulator();


function create_server(port, public_dir) {
	var app = express();
	
	function log(str) {
		console.log( port + ":" + public_dir + " " + str);
	}
	app.configure(function() {	
		app.use(express.errorHandler());
		app.use(express.logger('dev'));	
	
		app.use(express.json());
		app.use(express.urlencoded());
		app.use(express.methodOverride());
	
		app.use(app.router);
	
		app.use(express.static(public_dir));
	
		app.get("/", function(req, res){
			res.redirect("/index.html");
		})

		app.post("/api/:method_id", function(req, res ){
			var lookup = {	
				"0" : em.data_dump,
				"1" : em.read_reg,
				"2" : em.write_reg,
				"3" : em.read_mem,
				"4" : em.write_mem,
			};
		
			console.log(req.params.method_id);

			var creq = req.body.req;		
			log("======== request ========");
			console.dir(req.body);
		
			var jres = lookup[req.params.method_id].call(em, creq);		

		
			log("======== response  ========");
			console.dir(jres);
			res.contentType('application/json');		
			res.writeHead(200);
			res.write(JSON.stringify(jres));
			res.end()
		});
	});


	http.createServer(app).listen(port, function(){
		console.log('Express server listening on port ' + port + " with public path - " + public_dir);
	});
}

create_server(4000, path.join(__dirname, "public"));
create_server(4004, path.join(__dirname, "public-production"));


