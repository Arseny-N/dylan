
(function($){
	$.el_height = function(el) {
		return el.offsetHeight;
	}	
})($);

(function(app, _, $, saveAs) {

	app.utils = {};

/*	app.utils.gen_route = function(s) {
		var $e = $(s);
		return function() {		
			this.$current.slideUp();
			this.$current = $e;
			this.$current.slideDown();
		}
	}

*/
	app.utils.initialize = function () {
		app.utils.$currrentOp = $("#current-op span");
		app.utils.files.setup();
	} 


	app.utils.mk_templ = function (selector) {
		return function(args) {
		
			var templText = $(selector).html() 

			return _.template(templText, args);
		}
	}
	app.utils.copyToClipboard = function(text) {
		window.prompt("Copy to clipboard: Ctrl+C, Enter", text);
	}
	app.utils.currrentOp = function(op) {
		console.dir([app.utils.$currrentOp,op])
		/* if(op == '') {
			var e = app.utils.$currrentOp;
			app.utils.$currrentOp.animate({		
				opacity: 0,
			}, "slow", function(){
				e.html('');
				e.css("opacity", 1)
			});
			return;
		}*/ 
		
		app.utils.$currrentOp.html(op);
	}
	/*--------------------------------------------------------*/
	app.utils.files = {};
	app.utils.files.setup = function() {
		app.utils.files.$input = $("input#opened-documents");	// this ???
		app.utils.files.$cont = $('#file-hidden-input');
	}
	app.utils.files.load = function(Doc) {
		app.utils.files.$input.one("change", function(evt){
			evt.stopPropagation();
	    		evt.preventDefault();
	    			
	    		console.log("------------------------");
	    		var files = evt.target.files;
	    		console.dir(files);
	    		for (var i = 0, f; f = files[i]; i++) {    
	     	 		var doc = new Doc({ file : f, name : f.name });
	     	 		var reader = doc.disk_read(f, function(){
	     	 			app.docs.add(doc);
	     	 		});
			}			
		});
		console.log("------------");
		app.utils.files.$input.click();

		
	}
	app.utils.files.save = function(doc) {
	
		var blob = new Blob([doc.get("data")],{type: "text/javascript"});
		console.dir(blob);
		saveAs(blob, doc.get("name"));	
	}

	app.utils.__object2string = function __dir(o, radix, depth, count, tab, nl, index) {

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
	app.utils.object2string = function dir(o, opt) {
		opt = opt || {};
		return app.utils.__object2string(o, opt.radix || 10 , opt.depth || 16, 1, opt.tab || '\t', opt.nl || '\n', opt.index);
	}
})(app, _, $, saveAs);
/*--------------------------------------------------------*/
