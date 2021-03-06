[**< Back**](index.html)

# Low level API


This document describes how to to port the server to a specific board.

### API Types & Defines

#### base_t
All data is stored in `base_t` type.

#### MAX_STR_BASE_T
This define is equal to the maximum length of the string representation of a `base_t`.

#### addr_t
All data is addressed through the `addr_t` type.

#### MAX_STR_ADDR_T
This define is equal to the maximum length of the string representation of a `addr_t`.

#### MAX_MEM_BUF_LEN
This define is equal to the count of the buffers with witch the data will be transmitted 
via the mem functions. In other words every mem_read\write call will read\write `MAX_MEM_BUF_LEN`
`base_t` units or less.

### API Functions

A specific port should define a `lowlevel_api` structure, assigning it's 
fileds appropriate functions. 

```C

typedef void (*write_register_t)  (address_t addr, base_t value);
typedef void (*read_register_t)   (address_t addr,  base_t *value);


typedef void (*write_memory_t)  (address_t addr, base_t *value,  size_t len);
typedef void (*read_memory_t)   (address_t addr, base_t *value,  size_t len);

typedef int  (*take_func_t)   (void);
typedef void (*release_func_t)   (void);
typedef int  (*open_func_t)   (void);
typedef void  (*close_func_t)   (void);

struct lowlevel_api {
	
	take_func_t    take; 
	release_func_t release; 
	
	write_register_t reg_write;
	read_register_t  reg_read;
	
	write_memory_t mem_write;
	read_memory_t  mem_read;

	ssize_t (*base_to_string)(base_t num, char string[MAX_STR_BASE_T + 1]);
	int 	(*string_to_base)(char string[MAX_STR_BASE_T + 1], base_t *num);

	int (*string_to_addr)(char saddr[MAX_STR_ADDR_T + 1], address_t *addr);
	

	char *name;	
};

```
### Data i/o

#### lowlevel_api::reg_write
* **Type** : `void (*write_register_t)  (address_t addr, base_t value);`
* **Description**: when called this function should write to a register located at **addr** 
the data specified in **value**.


#### lowlevel_api::reg_read
* **Type** : `void (*read_register_t)   (address_t addr,  base_t *value);`
* **Description**: when called this function should read the register located at **addr**, 
the result should be returned in **value**.

#### lowlevel_api::mem_write
* **Type** : `void (*write_memory_t)  (address_t addr, base_t *value,  size_t len);`
* **Description**: when called this function should write to a memory location that starts at **addr** 
the data specified in **value**, which size is **len**.

#### lowlevel_api::mem_read
* **Type** : `void (*read_memory_t)  (address_t addr, base_t *value,  size_t len);`
* **Description**: when called this function should read the memory location that starts at **addr** 
the data should be stored in **value**, the amount of data to be read is passed through **len**.

### Type conversion

The server already have some default functions to convert strings to numbers and etc, but 
in case of an unusual type the following functions could be also specified.

#### lowlevel_api::base_to_string
* **Type** :`ssize_t (*base_to_string)(base_t num, char string[MAX_STR_BASE_T + 1])`
* **Description**: this function translates a `base_t` type to a string, 
the resulting string length is returned. Upon error a -1 return value is expected.

#### lowlevel_api::string_to_base
* **Type** :`int (*string_to_base)(char string[MAX_STR_BASE_T + 1], base_t *num)`
* **Description**: this function translates a string type to a `base_t`. 
Upon error a -1 return value is expected, upon successful execution a return value of 0 is expected.

#### lowlevel_api::string_to_addr
* **Type** :`int (*string_to_addr)(char saddr[MAX_STR_ADDR_T + 1], address_t *addr)`
* **Description**: this function translates a string type to a `addr_t`. 
Upon error a -1 return value is expected, upon successful execution a return value of 0 is expected.

### Init & Finit

#### lowlevel_api::open
* **Type** :`int  (*open_func_t)   (void);`
* **Description**: this function is called once - on the start of the web server. If it returns 
-1 then execution stopped. 

#### lowlevel_api::take
* **Type** :`int  (*take_func_t)   (void);`
* **Description**: this function is called before processing a POST request - if it returns -1 then 
the request is dropped.


#### lowlevel_api::close
* **Type** :`void  (*close_func_t)   (void);`
* **Description**: this function is called on server shutdown. (really ?)

#### lowlevel_api::release
* **Type** :`void (*release_func_t)   (void);`
* **Description**:  this function is called after every POST request.

#### lowlevel_api::name
* **Description**: Purely informational filed, not used anywhere, _could be omitted_.

### Example

``` C
struct lowlevel_api dummy_api = {
	
	.reg_write = dummy_write_register,
	.reg_read = dummy_read_register,
	
	.mem_write = dummy_write_memory,
	.mem_read = dummy_read_memory,

	.name = "dummy api"
};

struct lowlevel_api *API =  &dummy_api;
```

1. We set all the needed fields to our functions.
2. We set the `API` pointer to the needed "low level api" structure.
3. That's all !

The api's are exported through the `API` object so setting it to a valid pointer it's a must.

