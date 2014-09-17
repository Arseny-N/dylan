#ifndef __JSON_ZERO_H_
#define __JSON_ZERO_H_

#include <ctype.h>      /* for isdigit() and isspace() */
#include <stdlib.h>     /* for NULL */
#include "macros.h"     /* for likely/unlikely & confing */




#ifdef JSON_ERRORS 
 #include <stdio.h>
 #include <errno.h>
 #ifndef error
  #define error(fmt, args...) 		fprintf(stderr, __FILE__ ":%s:%d ERROR(%d): " fmt "\n", __func__, __LINE__,errno, ##args)
 #endif
 #define JSON_error(fmt, args...) 	error("json-error:" fmt, ##args)
 #define JSON_assert(msg, assert, handler) do{ if(assert){ JSON_error(msg); handler; }  } while(0)
#else 
 #define JSON_error(fmt, args...)
 #define JSON_assert(msg, assert, handler) (assert);
#endif


#define ERR    0x0
typedef unsigned int offset;
 
#ifdef JSONX_LWIP
 #include "network-stack.h"
 
 #define at(buf, ind) pbuf_get_at(buf->p, (ind))
 #define ok(buf, ind) ( (ind) < (buf->p->tot_len) )
 typedef struct netbuf xjson;
 
#else

 #define at(buf, ind) buf[ind]
 #define ok(buf, ind) (buf[ind] != 0)
 typedef char xjson;
 
#endif


/**
 *  index: the index of the array element
 *  *val : a value corresponding to the name, could be any JSON type including arrays and objects.
 *  vlen: length of the *val
 *  *arg : the pinter passed to the JSON_decode_object() function.
 *
 *  If the *val is an object or array, it could be parsed by the JSON_decode_* functions.
 *  
 **/
typedef int (*JSON_array_cb)  (xjson *b, int index, offset oval, int vlen,  void *arg);

/**
 *  *name: null terminated name
 *  nlen: length of the *name
 *  *val : a value corresponding to the name, could be any JSON type including arrays and objects.
 *  vlen: length of the *val
 *  *arg : the pinter passed to the JSON_decode_object() function.
 *
 *  If the *val is an object or array, it could be parsed by the JSON_decode_* functions.
 *  
 **/
typedef int (*JSON_object_cb) (xjson *b, offset oname, int nlen, offset oval, int vlen, void *arg);



int JSON_decode_array(xjson *b, offset off,  JSON_array_cb callback, void *arg);
int JSON_decode_object(xjson *b, offset off, JSON_object_cb callback, void *arg);
int JSON_decode(xjson *b, offset off, JSON_array_cb a_cb, JSON_object_cb o_cb, void *arg);


void print_xjson(char *prefix, char *postfix, xjson *b, offset off, int len);
/**
 * JSON_decode(char *s, JSON_array_cb a_cb, JSON_object_cb o_cb, void *arg)
 * JSON_decode_array(char *s, JSON_array_cb callback, void *arg)
 * JSON_decode_object(char *s, JSON_object_cb callback, void *arg)
 *
 * *s: 
 *	the first character should be a valid JSON one, the string should be null terminated.
 * callback:
 *	the callback function could call the JSON_decode_* funcs to decode an element.
 */

#define JSON_string_unbracet(start,  len)       do{	start += 1; len -= 1;	} while(0) 
#define JSON_string_bracet_restore(start,  len) do{     start -= 1; len += 1;	} while(0)



#define string_static(string) string, ((size_t)sizeof(string) -1 )

#endif
