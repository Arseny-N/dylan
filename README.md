# Dylan

Dylan is a remote debugger interface for _freeRTOS + lwip_ based systems. In order to use it the user 
is expected to implement a C API of several functions related to memory and register IO. Then 
he/she will be able to access these functions through the web interface and a json API. The web
interface allows to write javascript scripts in order to interact with the board.

This package includes:
  - an lwip based http-server made to provide a json API for the web interface and serve the web page
  - a one-page-app style web based debugging interface which includes a local-storage persistent web editor, console and log analysis system.
  - a build system to compress the web based debugging interface in under 120 Kbs and embed it in the C program
  
In order to facilitate development the package is runnable on a linux machine. For more information refer to the [documentation](http://arseny-n.github.io/dylan/).
