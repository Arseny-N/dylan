
[**< Back**](index.html)

# Console API 

## Built-in functions

### Memory i/o

The `m.<func>` set of functions allows you to interact with the memory of the microprocessor.

#### ` m.r(address)`
* **Description**: Reads a memory location.
* **Arguments**:	
 * `address`  [*Number*]  - a memory address from where data should be read.
* **Return Value**  [*Array*]   - the function returns the result of the memory read to the given location. 

#### ` m.w(address, data)`
* **Description**: Writes a memory location.
* **Arguments**:
 * `address`  [*Number*]  - a memory address where data should be written.
 * `data`     [*Array*] 	- data which should be written to the *address*.
* **Return Value** [*String*]   - the function returns the result of the memory read to the given location. 	 		 
* **Notes** : A successful write returns a *"success"* string.

---
### Register i/o

The `r.<func>` set of functions allows you to interact with the registers of the microprocessor.

#### ` r.r(register)`

* **Description**: Reads a register.
* **Arguments**:
 * `register` [*Untyped*] - a register from where data should be read.
* **Return Value** [*Number*]  - the function returns the result of the register read. 
	     
#### `r.w(register, data)`
* **Description**: Writes a register.
* **Arguments**:
 * `register` [*Untyped*]  - a register where data should be written.
 * `data`	   [*Number*] 	- data which should be written to the *register*.
* **Return Value** [*String*]   - the function returns the result of the register read.
* **Notes** : A successful write returns a *"success"* string.
	
---

#### `require(library)`
* **Description** : See [editor-api](editor-api.html#require-library-).

#### `error(message)`
#### `info(message)`
#### `warn(message)` 

* **Description** : See [logger](editor-api.html#logger).
* **Notes**	  : These functions are ruffly equal to `info = require('logger').info`, and so on.

## Helper libraries
All Libraries available to the editor scripts are also available to the console.
See [editor-api](editor-api.html).




