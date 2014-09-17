#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <error.h>
#include <string.h>

#include "network-stack.h"
#include "generic-json-zero-copy.h"

int cb_json_print_obj (xjson *b, offset oname, int nlen, offset oval, int vlen, void *arg);
int cb_json_print_arr(xjson *b, int index, offset oval, int vlen,  void *arg);

int cb_json_print_obj (xjson *b, offset oname, int nlen, offset oval, int vlen, void *arg)
{
	int depth = ((int) arg);
	printf( "%*s", depth * 8, "");
	print_xjson("name --> ", "\n", b, oname, nlen);
	
	if(at(b, oval) == '[' || at(b, oval) == '{' )	
		return JSON_decode(b, oval, cb_json_print_arr ,cb_json_print_obj, (void*)(depth + 1) );
	
	printf( "%*s", depth * 8, "");
	print_xjson("value--> ", "\n\n", b, oval, vlen);
	
	return 0;
}
int cb_json_print_arr(xjson *b, int index, offset oval, int vlen,  void *arg)
{
	int depth = ((int) arg);
	printf( "%*s", depth * 8, "");

	printf( "index --> %d\n", index);

	if(at(b, oval) == '[' || at(b, oval) == '{' )	
		return JSON_decode(b, oval, cb_json_print_arr ,cb_json_print_obj, (void*)(depth + 1) );
		
	printf( "%*s", depth * 8, "");
	print_xjson("value --> ", "\n\n", b, oval, vlen);
	return 0;
}

void print_pp(char *str,char *p, char *prefix) 
{
	printf("%s%*sV\n",prefix, p - str , "");
	printf("%s%s\n",prefix, str);
//	printf("%*s^\n", p - str , "");
}

struct netbuf *mk_netbuf(void *buf, int len) 
{
	struct netbuf *b = netbuf_new();
	if(!b) {
		error("netbuf_new()");
		return NULL;
	}
	
	if(netbuf_ref(b, buf, len) != ERR_OK) {
		error("netbuf_ref()");
		return NULL;	
	}
	return b;
}	
int main ( int argc, char * argv [] )
{

	char *res;
	int  len;
	
	
	if(argv[1] == NULL) {
		printf("%s URL\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	struct netbuf *nb = mk_netbuf((char*) argv[1] , strlen(argv[1]));
	if(!nb) {
		error("mk_netbuf()");
		exit(EXIT_FAILURE);
	}
	
	if(JSON_decode(nb, 0,cb_json_print_arr ,cb_json_print_obj, (void * ) 0 ) == -1) {
		fprintf(stderr, "JSON_decode() failed, bad json\n");
		exit(EXIT_FAILURE);
	}	
	
	exit(EXIT_SUCCESS);		

}

