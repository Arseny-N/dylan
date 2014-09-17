var expect = require("chai").expect;
var sapi = require("../api");


describe('api', function(){
	function bad_clone(o) {
		return JSON.parse( JSON.stringify( o ) );
	}
	describe('registers i/o', function(){
		var registers = [0, 1, 2, 3];
		var zero_registers = [4,5,6,7,8,9,1000];
		var rmap = {};			

		function write_regs( done, registers, rmap) {
			var regs = bad_clone(registers);
			var data;
			
			(function next(reg) {
				if(typeof reg === "undefined") 
					return done();

				data =  Math.round(Math.random() * 100);
				if(rmap)
					rmap[reg] = data;
				
				sapi.reg.write(reg, data, function(err, value){	

					if(err) return done(err);

					expect(value).to.be.equal("success");
					
					next(regs.pop())
				});
			})(regs.pop());
		}
		
		function read_regs(done, registers, rmap) {
			var regs = bad_clone(registers);
			var data;
			(function next(r){
				if(typeof r === "undefined") 
					return done();										

				sapi.reg.read(r, function(err, 	value){
					if(err) return done(err);
					if(rmap)
						expect(value).to.be.equal(rmap[r], "register " + r);
					else
						expect(value).to.be.equal(0)
					next(regs.pop());
				})


			})(regs.pop());	
		}
		it('writes should be always successfull', function(done){
			write_regs(done, registers, rmap);			
		})
		it('reads should return the written values', function(done){			
			read_regs(done, registers, rmap);
		})
		it('writes to non-existend registers should be always successfull', function(done){
			write_regs( done, zero_registers );			
		})
		it('reads on non-existent registers should return 0', function(done){			
			write_regs(done, zero_registers);
		})
	});
	
	
	
	function random_number(min, max) {	
		return  Math.floor(Math.random() * (max - min)) + min;
	}
	function random_array(minl, maxl,min, max) {
		var len =  random_number(minl, maxl);
		var a = [];
		while(--len) 
			a.push(random_number(min, max))
		return a;
	}
	
	describe('memory i/o # random #', function(){
		var memory = [];		
		before(function(){			
			var len = random_number(10, 400);
			var prev = 0;	
			var max_mem = 127;
			for(var i=0; i < len ; ++i) {
			
				var start = random_number(prev, prev + 4);
				var count = random_number(0, 10000);
				prev = start + count;
				if(prev > max_mem)
					break;
				memory.push({start : start, count:count});				
			}
			
		});
		it('writes should be always successfull', function(done){
			(function next(index) {
				var mem = memory[index];
				if(!mem) 
					return done();
				mem.data = [];
				for(var i = 0; i < mem.count; ++i ) {		
					var d = Math.round(Math.random() * 100) 
					mem.data.push( d );
				}
				sapi.mem.write(mem.start, mem.data, function(err, value){	
					if(err) return done(err);
					
					expect(value).to.be.equal("success");
										
					next(index+1)
				});
			})(0);
		});

		it('reads should return the written values', function(done){			
			var mem = bad_clone(memory);
	
			(function next(m){
				if(!m) 			
					return done();
				
				sapi.mem.read(m.start, m.count, function(err, data){
					if(err) return done(err);	
				
					expect(data).to.be.deep.equal(m.data);	
					next(mem.pop())
				});
			})(mem.pop());	
		});
	})
	
	describe('memory i/o # linear #', function(){
		var memory = random_array(50, 52, 10, 20);		
		it('writes should be always successfull', function(){									
			sapi.mem.write(0, memory, function(err, value){	
				if(err) return done(err);				
				expect(value).to.be.equal("success");				
			});

		});

		it('reads should return the written values', function(){			
			sapi.mem.read(0, memory.length, function(err, data){
				if(err) return done(err);	
				
				expect(data).to.be.deep.equal(memory);					
			});
		});	
	})

})
