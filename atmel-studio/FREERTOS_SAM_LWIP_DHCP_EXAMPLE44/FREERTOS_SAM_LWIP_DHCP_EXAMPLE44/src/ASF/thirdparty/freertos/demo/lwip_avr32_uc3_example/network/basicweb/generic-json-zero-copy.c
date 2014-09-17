#include <stdarg.h>


#include "generic-json-zero-copy.h"

void print_xjson(char *prefix, char *postfix, xjson *b, offset off, int len)
{
	int i;
	printf("len=%d,off=%d ",len, off);
	printf("%s", prefix);
	for(i=off; i < off+len && ok(b, i); ++i ) {
		char c = at(b, i);
		switch(c) {
			case '\r': printf("\\r"); 	break;
			case '\n': printf("\\n\n"); 	break;						
			case '\0': printf("\\0"); 	break;
			default:   printf("%c", c); 	break;
			
		}
	}
	printf("%s", postfix);
}

static offset __get_json_string_end(xjson *b, offset off) 
{
	offset i;
	char depth = 0;

	for(i=off; ok(b, i); ++i) {
		char c = at(b, i);

		if( c == '"' ) { 			
			depth = (depth == 0); 
			if(depth == 0) {
				if( i > 0 && at(b, i-1) == '\\' ) {		
					depth = (depth == 0); 
					continue;
				}
				JSON_assert("offset == ERR, bad ERR value!", i == ERR, return ERR;);
				return i;
			}
			continue; 
		}		
	}
	JSON_error("__get_json_string_end() failed to find `\"'");
	return ERR;
}
static offset __get_json_object_end(xjson *b, offset off) 
{
	offset i;
	char depth = 0;

	
	for(i=off; ok(b, i) ; ++i) {
		char c = at(b, i);
		if( c == '"' ) {
			i = __get_json_string_end(b, i);
			if(i == ERR) {
				JSON_error("__get_json_sting_end() failed");
				return ERR;	
			}
			continue;
		}
				
		if( c == '{' ) { depth ++; continue; }	
		if( c == '}' ) { 
			depth --; 
			if(depth == 0) { 
				JSON_assert("offset == ERR, bad ERR value!", i == ERR, return ERR;);
				return i;
			}
			continue; 
		}		
	}
	
	JSON_error("__get_json_object_end() failed to find `}'");
	return ERR;
}
static offset __get_json_array_end(xjson *b, offset off) 
{
	offset i;
	char depth = 0;

	for(i=off; ok(b, i) ; ++i) {
		char c = at(b, i);
		if( c == '"' ) {
			i = __get_json_string_end(b, i);
			if(i == ERR) {
				JSON_assert("offset == ERR, bad ERR value!", i == ERR, return ERR;);
				return ERR;		
			}
			continue;
		}
		
		if( c == '[' ) { depth ++; continue; }	
		if( c == ']' ) { 
			depth --; 
			if(depth == 0) 
				return i;
			continue; 
		}
		
	}
	JSON_error("__get_json_array_end() failed to find `]'");
	return ERR;
}
static offset __get_json_number_end(xjson *b, offset off) 
{
	int i;
	for(i=off; ok(b, i); ++i) {
		char c = at(b, i);
							
		if( !isdigit(c) && c != 'e' && c != 'E' 
				&& c != '-' && c != '+' && c != '.' ) {
				JSON_assert("offset == ERR, bad ERR value!", i-1 == ERR, return ERR;);
				return i-1;	
		}
	}
	JSON_error("__get_json_number_end() failed to find number end");
	return ERR;
}
static int __compare(xjson *b, offset off, char *str, int len)
{
	offset i;
	
	for(i=off; ok(b, i) && i-off < len && str[i-off]; ++i) {
		char c = at(b, i);
		if(str[i-off] != c)
			return str[i-off] > c ? 1 : -1; 
	}
	return 0;
}
static offset __get_json_bool_or_null_end(xjson *b, offset off) 
{
	if( __compare(b, off,  "true", 4) == 0 || __compare(b, off, "null", 4) == 0) {
		off += 3;
	} else if( __compare(b, off, "false", 5) == 0) {
		off += 4;
	} else {	
		JSON_error("__get_json_bool_or_null_end() bad json value");
		return ERR;		
	}
	JSON_assert("offset == ERR, bad ERR value!", off == ERR, return ERR;);
	return off;
}

static offset _get_json_element_end(xjson *b, offset off) 
{
	char c = at(b, off);
	return c == '"' ? __get_json_string_end(b, off) : 
	       c == '{' ? __get_json_object_end(b, off) : 
	       c == '[' ? __get_json_array_end(b, off)  : 
	       (isdigit(c) || c == '-' || c == '+') ? __get_json_number_end(b, off) : __get_json_bool_or_null_end(b, off);	
}


/*------------------------------------------------*/

static  offset __first_non_space(xjson *b, offset off) 
{	
	offset i;
	
	for(i = off; ok(b, i) && isspace(at(b, i)) ; ++i); 
	
	JSON_assert("offset == ERR, bad ERR value!", i == ERR, return ERR;);
	
	return ok(b, i) ? i : ERR;
}
static offset __get_next_elem(xjson *b, offset off, char skip_once)
{

	off = __first_non_space(b, off);
	if(off == ERR)
		return off;

	if( at(b, off) != skip_once ) 
		return off;
		
	JSON_assert("offset == ERR, bad ERR value!", off + 1 == ERR, return ERR;);
	off = __first_non_space(b, off+1);
	if(off == ERR)
		return off;

	return off;
}

/*------------------------------------------------*/

int JSON_decode_object(xjson *b, offset off, JSON_object_cb callback, void *arg)
{
	offset oname, oname_e;
	offset ovalue, ovalue_e;
	
	int rv = 0;
	
	for(; ok(b, off) && ok(b, off + 1) ;) {			
		oname = __get_next_elem(b, off+1, ',');
		if(oname == ERR) {
			JSON_error("__get_next_elem(1)");
			return -1;
		}
		if(at(b, oname) == '}' ) {
			return 0;
		}
		

		oname_e = __get_json_string_end(b, oname);
		if(oname_e == ERR) {
			JSON_error("__get_json_string_end()");
			return -1;
		}
		

		ovalue = __get_next_elem(b, oname_e+1, ':');
		if(ovalue == ERR) {
			JSON_error("__get_next_elem(2)");
			return -1;
		}

		ovalue_e = _get_json_element_end(b, ovalue);
		if(ovalue_e == ERR) {
			JSON_error("_get_json_element_end()");
			return -1;
		}
		

		rv = callback(b, oname, oname_e - oname + 1 , ovalue, ovalue_e - ovalue + 1, arg);		
		if(rv)
			return rv;
			

		off = ovalue_e;
	}
	return 0;
}

int JSON_decode_array(xjson *b, offset off,  JSON_array_cb callback, void *arg)
{
		
	offset ovalue, ovalue_e;
	int rv = 0, index = 0;
	
	for(; ok(b, off) && ok(b, off + 1) ;) {

		ovalue = __get_next_elem(b, off+1, ',');
		if(ovalue == ERR) {
			JSON_error("__get_next_elem()");
			return -1;
		}
		if(at(b, ovalue) == ']' ) {
			return 0;
		}

		ovalue_e = _get_json_element_end(b, ovalue);
		if(ovalue_e == ERR) {
			JSON_error("_get_json_element_end() ovalue=%lu, ovalue_e=%lx", (long)ovalue, (long)ovalue_e);
			return -1;
		}
		

		rv = callback(b, index++, ovalue, ovalue_e - ovalue + 1 ,arg);
		if(rv)
			return rv;
		
		off = ovalue_e;
	}
	return 0;
}
int JSON_decode(xjson *b, offset off, JSON_array_cb a_cb, JSON_object_cb o_cb, void *arg)
{
	char c = at(b, off);
	if(c == '{' && o_cb)
		return JSON_decode_object(b, off, o_cb, arg);
	if(c == '[' && a_cb )
		return JSON_decode_array(b, off, a_cb, arg);
	return -1;
}

