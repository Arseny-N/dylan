[**< Back**](index.html)

# Testing

This document describe various testing facilities offered by this project.
You can find them at _test-suite/_ directory.

## Server Testing

Server testing is done by the means of a test runner called **motcha**, and 
a test collection called **chai**.

Mocha and chai use Node.js, so you have to install it on your computer to run them.

To run all the tests go to _test-suite/mocha-tester/_ and run **mocha**. It will execute
the following test, using the JSON api offered by the web server :


* Write fixed valuers to registers and test weather the return value equals to "success".
* Read data form the registers and check weather it is correct (equal to the data previously written).
* Write random data to random addresses of memory and check weather the return value is "success".
* Read data from memory and compare to the previously written.


## UI Testing

Well there are no ui tests, but there is a web server made to facilitate development.
Go to _test-suite/node-server_ and run `node app`. You will get to web pages - one 
on `localhost:4000` and one on `localhost:4004`. The former is the development page - 
the server reads data form the _source_ directory, so everything is not minimized.
The latter is the development page , the server reads data from the _build_ directory, 
everything is minimized.

All register and memory i/o functionality is implemented.

