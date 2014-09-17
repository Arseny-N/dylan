Number.prototype.times = function(fn) { 
	if(fn) {
		var r, n = this.valueOf(); 	
		for(var i = 0; i< n; ++i)  {
			r = fn(i); 
			if( typeof r !== "undefined" )
				break;
		}
		return r;
	}
}

Boolean.prototype.once = function(fn) {
	var n = this.valueOf();
	
	if(fn && n) {
		this.value = false;
		return fn(); 			
			
	}
	return n;	
}

Boolean.prototype.once = function(fn) { var n = this.valueOf();	if(fn && n) {  this = false;return fn(n); } return n;	}

var fs = require("fs");
function extend(o1, o2) {
	for(var i in o2) {
		if(o2.hasOwnProperty(i))
			o1[i] = o2[i];
	}
	return o1;
}
(function(root) {

/*
 *	headers : {
 * 		<header_path> : {
 *			<define_name> : <import_name>
 *		}
 *	}
 */
 
 
	var Model = root.Model = function (options) {
		this.json_source = "package.json";
		this.line_regexp = new RegExp(/#define (\w+)\s+(\w+|\"\w+\"|'\w+')/);
		
		extend(this, options);	
		extend(this, this.json_parse(this.json_source));				
		extend(this, this.process_headers(this.headers));
		
	}
	Model.prototype.process_headers = function(hdrs){
		function process_header(hdr, parsed) {
			var res ={};
			for(var i in hdr) {
				if( hdr.hasOwnProperty(i) && parsed[i])
					res[hdr[i]] = parsed[i];
			}
			return res;
		}
		var res = {};
		for(var i in hdrs) {
			var hdr = hdrs[i];
			console.log("Processing: " + i);
			var res = this.hdr_parse(i);
			extend(res, process_header(hdr, res));
		}
		return res;
	}
	Model.prototype.get = function(option){
		return this[option];
	}
	Model.prototype.json_parse = function(file, encoding){		
		var data = fs.readFileSync(file, encoding).toString();
		return JSON.parse(data);
	}
	
	
	Model.prototype.hdr_parse_line = function(line) {		
		var r = this.line_regexp.exec(line);		
		
		return r ? { name : r[1], value : r[2] } : r;
		
	}
	Model.prototype.hdr_parse_string = function(string) {		
		var lines = string.split('\n');
		var rv = [];
		for(i in lines) {
			var line = lines[i];
			var res = this.hdr_parse_line(line);
			if(res)
				rv.push(res)
		}
		return rv;		
	}
	Model.prototype.hdr_parse = function(file, encoding){	
		var data = fs.readFileSync(file, encoding).toString();
		return this.hdr_parse_string(data);
	}	
})(exports)

function __dir(o, radix, depth, count, tab, nl, index) {

	function a_mt(arr, n) {
		var r = "";
		while(n--)
			r += arr;
		return r;
	}
	function is_err(o) {
		return o && o.__proto__ && o.__proto__.__proto__ == Error.prototype;
	}
	var string = "";

	if(depth == 0)
		return string + " ... ";		
	if(is_err(o))
		return o.toString();
	if(typeof o == "object") {
			if(o.__proto__ == Array.prototype) {
			string += '[';
			for(var i = 0; i < o.length; ++i) {
				if(o.hasOwnProperty(i)) {
					string += (i ? "," : "") + nl;
					if(index)
						string += "/* " + i +" */";					
					string += a_mt(tab, count);
					string += __dir(o[i], radix, depth - 1,  count + 1, tab, nl, index);
						
				}
			}
			return string + nl +a_mt(tab, count -1 ) + ']';
		
		} else {
			string += '{';
			var first = 1;
			for(var i in o) {				
				if(o.hasOwnProperty(i)) {
					string += (!first ? ", " : "") + nl;
					string += a_mt(tab, count);
					string += i + " : "+ __dir(o[i], radix, depth - 1, count + 1, tab, nl, index);
					first = 0;
				}
			}
			return string + nl + a_mt(tab, count -1 ) +'}';
		}
	}
	if(typeof o == "string") {
		return string + "\"" + o.toString(radix || 10) + "\"";
	}
	if(typeof o == "undefined") {
		return string + "undefined";
	}
	
	return string + o.toString(radix || 10);
}

