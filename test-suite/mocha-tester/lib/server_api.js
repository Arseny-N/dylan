var request = require('request');
var confing = require("../confing");

var api = {};

var gen = function(id) {
	return function(data, callback) {
		data = JSON.stringify(data);
		return 	request({
			uri: confing.URL + "/api/" + id,
			method: "POST",
			body 	: data,
			headers: {
//        			'Content-Lenght': data.length
			}
		}, function(error, response, body) {
			var e;
			
			body = JSON.parse(body);									

			var res = body ? body.result : undefined;
			return callback(error, res);
		});

	}
}

api.reg_read = gen("1");
api.reg_write = gen("2");
api.mem_read = gen("3");
api.mem_write = gen("4");
	
api.poll = gen("5");

exports.api = api;
	




