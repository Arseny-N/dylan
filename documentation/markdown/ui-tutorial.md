[**< Back**](index.html)

# UI Tutorial

This is a tutorial that describes the facilities offered by the web interface.

![screen shot](images/editor.png)

This project allows to interact with a board remotely. You can write a script 
specifying how the interaction should take place. The script should be written 
in the window **#2**. The output of the script will be displayed in the window
**#3**, the operation that is now running is placed in the window **#4**. If the log
output is too verbose it could be filtered with a regular expression typed in window **#1**.

The window **#3** have several "modes" :
* **Script debug** - quite verbose output, it informs what file was run, what has it exported and so on.
* **Chip debug** - less verbose output, the only data that will appear will be the data printed by the script.
* **Console**	- an interactive console.

The modes are changed by clicking on the tabs located at he top of window **#3**.
To clear the window **#3**, the button "Clear" should be pressed.

#### Buttons left to right

* **File** : opens a drop down menu, where you can choose one of the following actions:
 * **Open file**: open a file in the text editor.
 * **Create file**: create a file in the text editor. 
 * **Download**: download the currently viewed document.
 * **Save locally**: save the contents of the edited documents to the browser, using HTML5 local-storage.
 * **To clipboard** : copy to clipboard the currently viewed document.
 * **Close** : close the current document.
 * **Rename** : rename the current document.
* **Run**: runs the current document.

#### Types of documents (files)


There are two types of documents (files) recognized by the environment :

* Text documents
* Script documents

The difference is that text documents are made to keep data, so no highlighting is done while viewing them, 
but script documents are made to keep code so they are highlighted. You can execute a text document but you'll
get an error. Text documents are useful when manipulating data.

main.js:
```javascript
var api = require("api");
var doc = require("documents");
var board = require("sam3x");
var arr = doc.get_data("big-array.csv").split(",");

if(arr.lenght < 0 || arr.length > board.FLASH_SIZE ) 
	throw("Bad csv data");

api.mem.write(board.FLASH_START, arr);	
......
```

sam3x.js:
```javascript
exports.FLASH_START = 0x00a0000
exports.FLASH_END = 0x00aFFFF
exports.FLASH_SIZE = exports.FLASH_END - exports.FLASH_START;
......
```
 
 
big-array.csv:
```javascript
1,2,4444,3,4,5,5 .....
```




 
