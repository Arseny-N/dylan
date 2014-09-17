
[**< Back**](index.html)

# Server API 

All API calls are sent througt http POST requests.
Their results are sent as POST-back. 

----

#### Register I/O 
* **Read**
 * **Request URL**: `/api/1`
 * **Request Data**:
```json
{ 
	"req" : [ <register address> ]
}
```
  * **Result**:
   * **On error**: HTTP error code + error message
   * **On sucess**:
```json
{
	"result" : <register data>
}
```


* **Write**
 * **Request to**: `/api/2`
 * **Request Data**:
```json
{ 
	"req" : [<register address>, <register data> ]	
}
```
  * **Result**:
   * **On error**: HTTP error code + error message
   * **On sucess**:
```json
{
	"result" : "success"
}
```

----

#### Memory I/O 
* **Read**
 * **Request URL**: `/api/3`
 * **Request Data**:
```json
{ 
	"req" : [ <memory address>, <"base_t" count> ]	
}
```
  * **Result**:
   * **On error**: HTTP error code + error message
   * **On sucess**:
```json
{
	"result" : [ <data1>, <data2>, <data3>, <data4> ... ]
}
```


* **Write**
 * **Request URL**: `/api/4`
 * **Request Data**:
```json
{
	"req" : [ <memory address>, [ <data1>, <data2>, <data3>, <data4> ... ] ]
}
```
  * **Result**:
   * **On error**: HTTP error code + error message
   * **On sucess**:
```json
{
	"result" : "success"
}
```

