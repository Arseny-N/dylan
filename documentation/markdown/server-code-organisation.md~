[**< Back**](index.html)


# Server code organization and build system


#### Build system
 
This program could be build in two ways :
1. As a linux executable, using some ported to linux lwip routines. 
 * If the program is build as a linux executable then,  data sent to it, is stored in `pbuf`s,
netconn functions and friends are implemented with UNIX sockets. All data processing is done 
with lwip functions for `pbuf` processing. The ported lwip stack is located at pseudo-lwip/.
The building process is handled by the makefile (`make noos [-B]`).
2. As a freertos task using lwip.
 * If the program is build as a freertos task then,
the building process is handled by the programmer's copy and paste skills and the 
programming environment witch he is using.



#### Code organization

| File			                              |	Description			                                        |
| --------------------------------------------------- | ----------------------------------------------------------------------- |
| generic-json-zero-copy.c, generic-json-zero-copy.h  |	Callback based json parser.                                             |
| http_server.c http_server.h	 	              |	Server main function.		                                        |
| http-utils.c http-utils.h		              |	HTTP utilities used by the project	                                |
| json_decode.c		                              |	Test program for json parser, might be broken. (Not used by the server)	|
| low-level-api.c, low-level-api.h                    |	Low level API used by the server for memory and register i/o.	        |
| macros.h		                              |	Macroses used by the HTTP server.	                                |
| makefile		                              |	Makefile.	                                                        |
| network-stack.h	                              |	Header made to allow smooth pseudo-lwip to lwip transition.	        |
| noOS.c		                              |	Linux port of the server.	                                        |
| post_methods.c, post_methods.h                      |	Methods called by the server on POST requests.	                        |
| pseudo-lwip/		                              |	Pseudo-lwip (linux port).		                                |
| wp.h			                              | 	The web page		                                        |

#### JSON parser
* **Files**: generic-json-zero-copy.c generic-json-zero-copy.h
* **Description**: Here the callback json parser is located. More info about its usage could be found in the header.
* **Macros**: 
  * `JSON_ERRORS` : should be defined if various errors should be printed by the parser.
Uses `fprintf` or an `error` macro if defined.
  * `JSONX_LWIP` : if defined the parser is configured for ntbuf parsing.
Should be _always_ defined for correct functioning of the parser. 
* **Testing**: json_decode.c `make json`


#### HTTP server
* **Files**: http_server.c http_server.h  
* **Description**: Here the main server function is located - `void handle_request(struct netconn *netconn)` 
some header parser utilities and the `server_start()`, `server_stop()` functions.
* **Macros**: 
 * `MAX_URL_LEN` : the maximum length of a URL specified to the server.
Example: 196.254.44.44/URL - the length of URL should be less then `MAX_URL_LEN`.


#### HTTP Utils
* **Files**: http-utils.h http-utils.c	 
* **Description**: Here some http utilities are located. Most of them concern sending data to the connected client.
* **Macros**:
 * `NETCONN_PRINTF_BUF` : the size of the buffer of `netconn_printf`. Should not be huge, because used many times to send small amounts of data.

#### Low Level API
* **Files**: low-level-api.c low-level-api.h  
* **Description**: Here functions for memory and register i\o are stored, more about them could be found [here](low-level-api.html).

#### Post methods
* **Files**: post_methods.c  post_methods.h
* **Description**: Here functions for processing post requests are stored. They are the principal users of the json parser.
How the request routing works:

```

---[POST request to /api/2]---------------------\
                                                |
           +---------------------------+        |
           | post request lookup table |        |
           |                           |        |
           |     (method_index: 1)[]   |        |
        /--|---<-(method_index: 2)[]<--|--------/
        |  |     (method_index: 3)[]   |
        |  |	      .....            |
        |  |                           | 
        |  +---------------------------+
        |
        V
   [API->take()] ----------(if error)----------> [return error to client]
        |                                                ^ 
        |                                                |
        V                                                |
   method([request body])                                |
 { send data to the user }  -----------------------------/
        |
        |
        |
        V
  [API->release()]
	      
```

A POST request is done to /api/2, the second method is looked up in the _request lookup table_. 
Then `API->take()` it is run, then the needed **method** is run passing it the POST body.
In the end `API->release()` is run. More in `API->release()` and `API->take()` could be found [here](low-level-api.html)

#### Web Page
* **Files**: wp.h
* **Description** : This file is generated by `tools/f2h`, it contains a gzipped web page.

#### Makefile
* **Files**: makefile
* **Description**:  the makefile.
* **Targets** : 
 * **noos**: build the linux port.
 * **clean**: clean the project dir.
 * **valgrind**: run the linux port with valgrind tracker.
 * **json**: build the json parser testing program. 
	  


#### Debugging
* **Files**: macros.h
* **Description** : This header defines a few useful macros and some debugging functions. 
* **Macros**:
 * `DEBUG` : if defined, then print functions actually print something, otherwise they are defied as dummies.
 

#### Smooth lwip porting
* **Files**: network-stack.h noOS.c
* **Description** : depending on the configuration network-stack.h header includes or lwip headers or ported to linux
headers.
* **Macros**:
 * `STRICT` : if this macro is defined, then the program is compiled to use strict lwip.
Otherwise it is compiled as a port to Linux.
* **pseudo-lwip/**: 

| File			    |	
| :-------------------------: |	 
|    api.h		    |	
|    api_lib.c		    | 	
|    netbuf.c		    |
|    netbuf.h		    |
|    pbuf.c		    |	
|    pbuf.h		    |	
