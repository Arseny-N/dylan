/*
app.ajax = function(options) {
	function merge(obj2, obj) {
		for(o in obj)
			if(obj.hasOwnPropriety(o))
				obj2[o] = obj[o];			
	}
	var defaults = {
		url : "bad.url.dot.com"
		
	};
	
	merge(this, defaults); 
	merge(this, options);  
}
app.ajax.prototype.post = function __ajax_post(_data, callback) {
	function make_string(dt) {
		if( typeof dt == "object" )
			return JSON.stringify(dt);
		if( typeof dt == "function" )
			return make_string( dt() );
		if( typeof dt == "number" )
			return dt.toString ? dt.toString() : undefined;
		return dt; 
	}
	var req = new XMLHttpRequest();
	var data = make_string(_data);
	req.open("POST", this.url, true);
	
	req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	req.setRequestHeader("Content-length", data.length);
	req.setRequestHeader("Connection", "close");
	
	req.onreadystatechange = function() {
		if(req.readyState == 4 && req.status == 200) {
			
	}
	req.send();
	return req;
}
*/

/*--------------------------------------------------------*/
app.RWMemView = Backbone.View.extend({
	el : "#rw-mem",
	events : {		
		"change input[type=radio]" : "change_op",
		"click button" : "submit",
	},
	initialize: function (options) {
		this.options = options || {};						
		this.$addr = this.$("input#addr");
		this.$offset = this.$("input#offset");
		this.$count = this.$("input#count");		

		this.$rRadio = this.$("input[type=radio][value=read]");
		this.$wRadio = this.$("input[type=radio][value=write]");	
				
		
		this.$fname = this.$("span#file-name");
		this.listenTo(app.docs, "change:file", function(doc){
			this.$fname.html(doc.get("name"));
			this.doc = doc;
		});
		
		
	},	
	change_op : function(e) {
		console.log("CHANGE");
		this.op = e.target.value;
	},
	submit_read : function(){
		var addr = this.$addr.val();
		var count = this.$count.val();
		var self = this;
											
		app.libs.api.mem.read_u8(addr, count, function(err, resp) {
			var data = '';			
			if(err) {
				app.log("rwmemory:submit_read", 
					"error occured while sending read request", "error");
				return;
			}
			for(var i=0; i < resp.length; ++i) {
				data += String.fromCharCode(resp[i]);

			}
			app.log("rwmemory:submit_read", 
					"read operation completed succesfully");

			self.doc.set("data", data);
			
		});
	},
	submit_write : function(){
		var addr = this.$addr.val();
		var offset = this.$offset.val();
		var count = this.$count.val();
		
		
		data = this.doc.get("data");
		if(offset)
		 	data.substr(parseInt(offset));
		 console.log("data " + offset  )
		
		app.libs.api.mem.write_u8(addr, data, function(err, resp){
			if(err) {
				app.log("rwmemory:submit_write", 
					"error occured while sending write request", "error");
				return;
			}
			app.log("rwmemory:submit_write", 
					"write operation completed succesfully");
			console.dir(resp);
		})
	},
	submit : function(evt) {
		evt.preventDefault();
		if(this.$rRadio.prop("checked"))
			return this.submit_read();
			
		if(this.$wRadio.prop("checked"))
			return this.submit_write();

			
	}	
	
});
/*--------------------------------------------------------*/
app.RWRegView = Backbone.View.extend({
	el : "#rw-reg",
	events : {		
		"change input[type=radio]" : "change_op",
		"click button" : "submit",
	},
	initialize: function (options) {
		this.options = options || {};						
		this.$name = this.$("input#name");
		this.$value = this.$("input#value");
		
		this.$rRadio = this.$("input[type=radio][value=read]");
		this.$wRadio = this.$("input[type=radio][value=write]");	
				
	},	
	change_op : function(e) {		
		this.op = e.target.value;
	},
	submit_read : function(){
		var name = this.$name.val();
		var value = this.$value.val();
		var self = this;
											
		app.libs.api.reg.read_u(name,  function(err, resp) {
			var data = '';			
			if(err) {
				app.log("rwregister:submit_read", 
					"error occured while sending read request", "error");
				return;
			}
			for(var i; i < resp.length; ++i) 
				data += String.fromCharCode(resp[i]);
			app.log("rwregister:submit_read", 
					"read operation completed succesfully");
					
			app.log("rwregister:result", resp);
			console.dir(resp);
		});
	},
	submit_write : function(){
		var name = this.$name.val();
		var value = this.$value.val();
		var self = this;		
		
		app.libs.api.reg.write_u32(name, value, function(err, resp){
			if(err) {
				app.log("rwregister:submit_write", 
					"error occured while sending write request", "error");
				return;
			}
			app.log("rwregister:submit_write", 
					"write operation completed succesfully");
			console.dir(resp);
		})
	},
	submit : function(evt) {
		evt.preventDefault();

		console.dir([this.$rRadio, this.$wRadio]);	

		if(this.$rRadio.prop("checked"))
			return this.submit_read();
			
		if(this.$wRadio.prop("checked"))
			return this.submit_write();

		console.log("WRONGGGG");			
	}	
	
});
/*--------------------------------------------------------*/


 <!-- Modal -->
 <!--
 	title : modal title
 	body  : modal body
 	
 -->
<script type="text/template" class="template modal">	
	
	<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
  		<div class="modal-dialog"><div class="modal-content">
      			<div class="modal-header">
        			<button type="button" class="close" data-dismiss="modal">
        				<span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>
        			<h4 class="modal-title" id="myModalLabel"><%- title %></h4>
      			</div>
      			<div class="modal-body">
      				<%- body %>		
      			</div>
      			<div class="modal-footer">
        			<button type="button" class="btn btn-default" data-dismiss="modal">Close</button>        			
      			</div>
    		</div></div>
	</div>
</script>


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
		return e.toString();
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
function dir(o, opt) {
	opt = opt || {};
	return __dir(o, opt.radix || 10 , opt.depth || -1, 1, opt.tab || '\t', opt.nl || '\n', opt.index);
}
json = [
  {
    "_id": "53dcccfadd13da5c7c493438",
    "index": 0,
    "guid": "ed0a83dc-db8b-4321-aa71-1b169472e842",
    "isActive": true,
    "balance": "$2,570.57",
    "picture": "http://placehold.it/32x32",
    "age": 28,
    "eyeColor": "brown",
    "name": "May Moody",
    "gender": "female",
    "company": "UNI",
    "email": "maymoody@uni.com",
    "phone": "+1 (909) 458-3574",
    "address": "920 Alton Place, Lindisfarne, Virginia, 6027",
    "about": "Dolore magna esse pariatur ex nostrud. Qui est cillum proident adipisicing magna cupidatat et. Ut exercitation in dolore ea labore mollit esse id aute quis incididunt irure et. In laboris eu ex ad elit fugiat voluptate consectetur. Sit exercitation commodo ut excepteur ex eiusmod minim nostrud. Incididunt reprehenderit exercitation dolore consequat aliquip nostrud officia. Adipisicing exercitation non nulla aliqua quis fugiat do cupidatat aute.\r\n",
    "registered": "2014-06-02T08:58:05 -04:00",
    "latitude": 85.845512,
    "longitude": -58.194496,
    "tags": [
      "tempor",
      "aliqua",
      "nisi",
      "aliquip",
      "proident",
      "ut",
      "aliqua"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Fern Wallace"
      },
      {
        "id": 1,
        "name": "Cindy Wilson"
      },
      {
        "id": 2,
        "name": "Lessie Cannon"
      }
    ],
    "greeting": "Hello, May Moody! You have 2 unread messages.",
    "favoriteFruit": "strawberry"
  }
]
e = new TypeError("blah bah");
console.dir(e.toString());
s= dir(e);
console.log(s);
