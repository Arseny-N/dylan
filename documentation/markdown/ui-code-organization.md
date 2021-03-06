[**< Back**](index.html)


# Web Ui code organization and build system

The User interface could be divided into two parts - the web page and the build system.
The task of the build system is to assemble a web page from the sources making it as small as possible.
First we describe the packages used by the web page, then the packages used by the build system and 
the actions witch it performs while assembling the web page. In the end we are going to take a 
look at the code organization.

## The web page 

#### Javascript libraries used by the web page
* [jQuery](http://jquery.com) 	- moderately light helper for manipulation of DOM elements.
* [Bootstrap](http://getbootstrap.com) - front end framework, utilized for nice and clear styles.
* [Backbone.js](http://backbone.org)	- MVC like system for javascript, used to implement complex
dynamic elements.
 * [Backbone.localstorage]() 	- HTML5 local storage facility ported to backbone.
* [Underscore.js](http://underscore.org) - sole dependency of backbone.js, provides extended features for 
default javascript Objects. 
* [Codemirror](http://codemirror.net)  - text editor, the most light for now and most full and flexible option.
* [Mousetrap](http://mousetrap.org)    - the lightest javascript library used for keyboard shortcuts.
* [saveFile]()			- HTML5 file manipulations

## Build System

### How it works
The build system heavily uses [Node.js](nodejs.org), Node.js ports javascript on the server side
making it quite flexible language that could be used as a language for system programming.

The main task of the build system is to minimize and concatenate files located in the 
**source** sub directory moving them int the **build** directory.

The resulting file is **index.min.html**, it is then transformed to a C header by `tools/f2h`, 
or actually by `cd tools; make header [-B]`.The following section describes how the build 
system minimizes and combines various files located under the **source** directory.

### Package it uses
* [Gulp]() - streaming build system, in other words a kind of **make** for the web.
 * gulp-* - gulp wrappers for other node packages or just small gulp dependencies.
* [ejs]() - javascript templater used to create an index.html form the templates located at _templates/_
* [uglifyjs]() - javascript minifier.
* [htmlmin]() - html minimizer.
* [minifycss]() - css minifier.

## Building

Gulp works in the following way - it reads the `gulpfile.js` located in the current directory 
and executes it. The user can choose what to run by specifying some command line arguments, 
such command line arguments are called _targets_. If **gulp** is run with no arguments then 
the default target is executed. The following targets are avaible.

* **scripts** - minifies all javascripts moving them to _/javascripts/minified_, then concatenates and 
minifies them again dumping the result to _build/scripts.min.js_
* **styles** - minifies all styles moving them to _/stylesheets/minified_, then concatenates and 
minifies them again dumping the result to _build/styles.min.js_
* **html.dev** - builds an index.html file located at _source/_. Useful while developing - 
all scripts and styles are included via `<script>` and `<link>` tags, making debugging easier.
The tags point to full (not minified) javascripts, leading to a smoother debugging experience.
* **html.build** - builds a index.min.html located at _build/_ , all styles and scripts are embedded
so the web page could be served with one big HTTP GET request.
* **documentation.html** - build the documentation from markdown to html.

## Code organization

#### Backbone intro
Backbone.js ports some object orientation to the javascript world, the directory and code organization
are not highly tied to the abstraction provided by Backbone but some understanding is still
required.

Backbone is an _MVC_ framework made to allow easy development of complex solutions. 
It uses the following terminology:
* **Model** - an object that is designed to keep data - models implement validation and many
quite useful functions.
* **Collection** - an array of models. For instance data of an article would be kept in an model
but all the articles would be kept in a collection.
* **View** - an object that handles rendering of a model or collection associated to it.

So the MVC in the Backbone case stand for **M**odel **V**iew **C**ollection, usually the C is 
for Controller - the glue that tides the data (Models) and it's representation(View) together.

Also Backbone adds **Events** to the world of javascript. Everything could be an event emitter,
and everybody could receive it. Event provide an alternative to callbacks and direct function
calls. When a model is added to a collection an event is spawned when a model is removed an event is also spawned etc.

#### How everything works

Our web application keeps a model for every opened document, all of them are kept in the `app.docs`
collection. The data is not kept directly in the model but in an object prided by codemirror.
All the code related to the "documents" is located in _documents.js_ file. Data kept in `app.Document`
models is rendered with codemirror views (`app.ScriptEditor`). 

The code (data) kept in documents is interpreted with the `app.interpreter`, it `eval()`s code
supplied to it, an adds the `require()` functions and `exports` object. Also the `app.interpreter`
is used by  the console to process it inputs. Code related to `app.interpreter` is located
in _interpreter.js_. The interpreter does not use any of the functionality provided by Backbone.

The output of the interpreter is dumped to the log subsystem, each log message is kept in
a separate model and all models are kept in the `app.logs.raw` collection. The filtering is 
possible due to the existence of `app.logs.filtered` collection where the messages are put
only after filtering them. If no filtering is imposed then `app.logs.filtered` is equal to 
`app.logs.raw`.

## Directory organization


#### source/
| Directory	   | Description    |
| ---------------- | -------------- |
| fonts/	   | Here bootstrap icons are kept, not actually used. |
| javascripts/     | Here _javascript_ files are kept. |
| stylesheets/	   | Here _css_ files are kept. |
| templates/	   | Here templates files are kept for building **index.html** |
| index.html	   | Actually index.html |
 
 
 
| Directory	   | Description    |
| ---------------- | -------------- |
| api.js  	   | Warpers for AJAX calls      | 
| app.js  		| Main file, keeps text-editor views and the "main" view |
| console.js  		| Console related code |
| documents.js 		| Document related code  |
| full/  		| Full (not minimized) javascripts, kept for debugging |
| interpreter.js  	| interpreter related code |
| logs.js  		| Logging related code |
| minified/  		| Monified versions of the code |
| standard-libs.js  	| Default librarises accesible at run time byu the user edited scripts |
| utils.js		| Utilites  |




