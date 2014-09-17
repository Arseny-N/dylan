
[**< Back**](index.html)

# Editor API 

## Built-in functions
#### `reqire(library)` 

* **Description**: You can access api functions by `require()` ing a certain library.
Also you can `require()` a *script* file opened in the editor, the `.js` extension could be omitted.

* **Arguments**:
 * `library`  [*String*]  - the library which should be required
				 
* **Return Value**  [*Object*]   - the *exports* object of the library.

```javascript
var foolib = require("foo-lib");
```

#### `exports`

* **Description**: All library functions and variables are exported through the *exports* Object.
By default `exports == {}`.


####examples
```javascript

/* CatzLib.js */
var log = require('logger');

exports.greet = fucntion(what) {	
	log.info("Meow " + what);
}

/* Sctript.js */

var cat = require("CatzLib");

cat.greet("John");  // Will output "Meow John"

```
## Helper libraries
### logger 
The logger functions allow you to log information in the logger window,
there are three functions witch accept one argument - a _string_ to be logged on the window.
You can also pass an object which will be converted to a string.

#### `logger.dir(object, options)`
	
* **Description**: Converts an object to a string.	
* **Arguments**:	
 * `object`  [*Object*]  - the object which should be converted.
 * `options` [*Object*]  - an options object. It's fields include:
   * `radix` [*Number*] - the base to witch numbers in the object should be converted. By default 10.
   * `depth` [*Number*] - the maximum depth of the object, if set to -1, ignored. By default 16.
   * `tab` 	 [*String*] - the tab character used when the data is outputted.
   * `nl` 	 [*String*] - the new line character used when the data is outputted.
   * `index` [*Boolean*]- if set to true - output an index before each *Array* element commented out with c-style comments. _WARNING!_ no comments are allowed in the JSON notation. 			 
* **Return Value** [*String*]   - a string representation of *object*.
	
#### `logger.info(message)`
#### `logger.error(message)`
#### `logger.warn(message)`
	
* **Description**: Logs a *message* with appropriate priority.
* **Arguments**:	
 * message [*Anything*] - a message which should be logged.

```javascript

var log = require('logger'),

log.info("Hello World");
log.info({"hello":"word"});

```
### documents 
The documents functions allow you to manipulate file contests.
The files should be opened in the editor.

#### `documents.list()`
	
* **Description**: Lists all the available documents. (a little bit useless)	
* returns [*Array*] - an array of strings.

#### `documents.get_data(docname)`
	
* **Description**: Gets the contests of a document.
* **Arguments**:	
 * `docname` [*String*] - a name of the document.
* **Return Value** - the contests of the document (*docname*).
	
#### `documents.set_data(docname, data)`
	
* **Description**: Sets the contests of a document.
* **Arguments**:	
 * `docname` [*String*] - a name of the document.
 * `data` 	  [*String*] - data which should be written to *docname*.

```javascript
var docs = require('documents');
		
/* Get the names of all the opened files */
var docNames = docs.list();

/* Get the data from great-file.txt (it should be opened in the editor) */
var greatData = docs.get_data("great-file.txt") 			 

/* Append the data to great-file.txt (it should be opened in the editor) */
docs.set_data("great-file.txt", greatData + " End Of Great File") 
```

### Underscore.js
For complex javascript operations a helper library could be included - [underscore.js](http://underscorejs.org/)
```javascript
var _ = require('_');
var obj = { 
	dogs : "bark"
}

_.extend(obj, {
	hello : "word",
	cats  : "jump"
});

// obj is now { dogs : "bark", cats : "jump", hello : "word" }

```

## Server interaction libraries

The *api* library allows you to send and recive data from and to the server.
You can require the *api* library in the following way:

```javascript
var api = require('api');

api.mem.write(0x11, [2,3,4,5,56]);

```
### api.mem

The *api.mem* set of functions allows you to interact with the memory of the microprocessor.

#### `api.mem.read(address, callback)`

* **Description**: Reads a memory location.
* **Arguments**:	
 * `address`  [*Number*]  - a memory address from where data should be read.
 * `callback` [*Function*]- a callback function which will be called when the operation will complete.
The function should take two arguments - *err* and *res*, if an error occurs
the *err* argument is filled with an error message otherwise the result is 
stored in the *res* argument.
				 
* **Return Value**  [*Array*]   - the function returns the result of the memory read to the given location. 
The return value is equal to the *res* argument of *callback*.
	     
#### `api.mem.write(address, data, callback)`

* **Description**: Writes a memory location.
* **Arguments**:
 * `address`  [*Number*]  - a memory address where data should be written.
 * `data`	   [*Array*] 	- data which should be written to the *address*.
 * `callback` [*Function*]- a callback function which will be called when the operation will complete.
The function should take two arguments - *err* and *res*, if an error occurs
the *err* argument is filled with an error message otherwise the result is 
stored in the *res* argument.
	
* **Return Value** [*String*]   - the function returns the result of the memory read to the given location. 
The return value is equal to the *res* argument of *callback*.
	 		 
* **Notes** : A successful write returns a *"success"* string.
	
### examples

```javascript
var SOME_ADDR = 0x4356;
var data = [0x1, 0x2, 0x3, 0x4, 0x5];

var api = require('api');
var log = require('logger');

api.mem.write(SOME_ADDR, mem_data);
data = api.mem.read(SOME_ADDR+2, 2);

log.info("mem.read result = " +	log.dir(data)); // Should output "mem.read result = [0x3, 0x4]"

```	
-----

### api.reg

The *api.reg* set of functions allows you to interact with the registers of the microprocessor.

#### `api.reg.read(register, callback)`

* **Description**: Reads a register.
* **Arguments**:
 * `register` [*Number*] - a register from where data should be read.
 * `callback` [*Function*]- a callback function which will be called when the operation will complete.
The function should take two arguments - *err* and *res*, if an error occurs
the *err* argument is filled with an error message otherwise the result is 
stored in the *res* argument.
			 
* **Return Value** [*Number*]  - the function returns the result of the register read. 
The return value is equal to the *res* argument of *callback*.
	     
#### `api.reg.write(register, data, callback)`
* **Description**: Writes a register.
* **Arguments**:
 * `register` [*Number*]  - a register where data should be written.
 * `data`	   [*Number*] 	- data which should be written to the *register*.
 * `callback` [*Function*]- a callback function which will be called when the operation will complete.
The function should take two arguments - *err* and *res*, if an error occurs
the *err* argument is filled with an error message otherwise the result is 
stored in the *res* argument.	
* **Return Value** [*String*]   - the function returns the result of the register read. 
The return value is equal to the *res* argument of *callback*.		 		 
* **Notes** : A successful write returns a *"success"* string.
	
#### `api.poll(register, mask, value, ntimes)`
* **Description**: Polls a register.
* **Arguments**:
 * `register` [*Number*]  - a register which should be polled.
 * `mask`   [*Number*] 	- a mask which should be applied to *register*, and _value_.
 * `value` [*Number*] - value witch is expected at *register*.
 * `ntimes` [*Number*] - count of times functions should be run, default 50.
* **Return Value** [*None*]   - the function do not return any value.
* **Notes** : **!!this function may hang your browser!!** this function is implemented on the client side the main loop looks like this
``` javascript
{
	reg_val = api.reg.read(address);
} while((reg_val & mask) != (value & mask));
```

#### `api.wait(address, mask, value, callback, delay, ntimes)`
* **Description**: Polls a register, but uses timers, *callback* is madnatory.
* **Arguments**:
 * `register` [*Number*]  - a register which should be polled.
 * `mask`   [*Number*] 	- a mask which should be applied to *register*, and _value_.
 * `value` [*Number*] - value witch is expected at *register*.
 * `callback` [*Function*] - the callback function called upon success, its sole argument is the value
of the polled register after the last read.
 * `delay` [*Number*] - number of milliseconds to wait until next poll.
 * `ntimes` [*Number*] - count of times functions should be run, default 50. If a negative 
value is specified then the function will be run until success.	
* **Return Value** [*None*]   - the function do not return any value.


### examples

```javascript
var SOME_REG = 12;
var data = 5;

var api = require('api');
var log = require('logger');

api.reg.write(SOME_REG, data);
data = api.reg.read(SOME_REG);

log.info("reg.read result = " +	data); // Should output "mem.read result = 5"

```
