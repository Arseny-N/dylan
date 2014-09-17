var sapi = require("./lib/server_api").api;

var __gen_cb = function(ok, fn) {
	fn = fn || function(){};
	return function(error, body) {
		return fn(error, body);
	}		

};

exports.mem = {};
exports.reg = {};

exports.mem.write = function(address, data, fn) {
	var req = [ address, data ];
		
	if(typeof address != 'number' ) 
		throw TypeError("`address' and `count' should be a number");		
	if(data.__proto__ != Array.prototype ) 
		throw TypeError("`data' should be an array of numbers");		
	for(var i=0; i< data.length; ++i)
		if(typeof data[i] != 'number')
			throw TypeError("`data' should be an array of numbers");
	

	sapi.mem_write({req : req}, __gen_cb(1, fn))
}
exports.mem.read = function(address, count, fn) {
	var req = [ address, count ];

	if(typeof address != 'number' || typeof count != 'number') 
		throw TypeError("`address' and `count' should be a number");

	sapi.mem_read({req : req}, __gen_cb(1, fn))
}	


exports.reg.read = function(reg, fn) {		
	var req = [ reg	];
	if(typeof reg != 'number' ) 
		throw TypeError("`reg' should be a number");

	sapi.reg_read({req : req}, __gen_cb(1, fn))
}

exports.reg.write = function(reg, data, fn) {			
	var req = [ reg, data ];
	
	if(typeof reg != 'number' ||  typeof data != 'number') 
		throw TypeError("`data' and `reg' should be a number");

	
	sapi.reg_write({req : req}, __gen_cb(1, fn))

}	


