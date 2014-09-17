#include <string.h>

#include "macros.h"
#include "network-stack.h"

#include "post_methods.h"
#include "low-level-api.h"

#include "http-utils.h"

int cb_json_print_obj (xjson *b, offset oname, int nlen, offset oval, int vlen, void *varg)
{
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	int depth = ((int) arg->m->data);
	
	printf( "%*s", depth * 8, "");
	print_xjson("name --> ", "\n", b, oname, nlen);
	
	if(at(b, oval) == '[' || at(b, oval) == '{' ) {
		arg->m->data = (void *)(depth + 1);
		return JSON_decode(b, oval, arg->m->parse_array ,arg->m->parse_object, varg);
	}
	
	printf( "%*s", depth * 8, "");
	print_xjson("value--> ", "\n\n", b, oval, vlen);
	
	return 0;
}
int cb_json_print_arr(xjson *b, int index, offset oval, int vlen,  void *varg)
{
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	int depth = ((int) arg->m->data);

	
	printf( "%*s", depth * 8, "");

	printf( "index --> %d\n", index);

	if(at(b, oval) == '[' || at(b, oval) == '{' ) {
		arg->m->data = (void *)(depth + 1);
		return JSON_decode(b, oval,arg->m->parse_array ,arg->m->parse_object, varg);
	}
		
	printf( "%*s", depth * 8, "");
	print_xjson("value --> ", "\n\n", b, oval, vlen);
	return 0;
}

/*-----------------------------------------------------------*/
/* Register I/O */

struct reg_s {
	address_t addr;
	base_t value;	
	
	int ok;
} register_buf = {};

/**
 *    [ address ]
 */

int cb_read_reg(xjson *b, int index, offset oval, int vlen,  void *varg)
{
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	char saddr[MAX_STR_ADDR_T + 1];
	address_t addr;
	int i;	
	struct reg_s *reg = &register_buf;

	switch(index) {
		case 0: /* Address */
			for(i=0; i < MAX_STR_ADDR_T && ok(b, i + oval); ++i)
				saddr[i] = at(b, oval + i);		
			saddr[MAX_STR_ADDR_T] = '\0';
	
			if( string_to_addr(saddr, &addr) == -1 ) {		
				req_err(arg->req,401,"Bad reg address request");
				memset(reg, 0, sizeof(struct reg_s));
				return -1;
			}
			reg->addr = addr;
			reg->ok = 1;
			return 0;	
		default: /* Should not happen */
			req_err(arg->req,401,"Bad reg write request");
			memset(reg, 0, sizeof(struct reg_s));
			return -1;						
	}
	
	
}
int execute_read_reg(struct post_method *self, struct http_request *req)
{
	char svalue[MAX_STR_BASE_T + 1];
	base_t value;
	struct reg_s *reg = &register_buf;
	ssize_t len;
	
	if(!reg->ok) {
		req_err(req,401,"Bad reg write request");
		memset(reg, 0, sizeof(struct reg_s));
		return -1;
	}
	
	value = (base_t) 0;
	dbg("here");
	read_register(reg->addr, &value);
	dbg("here2");
	memset(reg, 0, sizeof(struct reg_s));
	
	len = base_to_string(value, svalue);
	if( len == -1 ) {		
		req_internal(req,"Bad reg address request");
		return -1;
	}
	svalue[MAX_STR_BASE_T] = '\0';
	

	if(reply_request(req, svalue, len)) {
		req_internal(req,"read_request_body()");
		return -1;		
	}

	return 0;
}

/**
 *    [ address, value ]
 */


int cb_write_reg(xjson *b, int index, offset oval, int vlen,  void *varg)
{
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	char saddr[MAX_STR_ADDR_T + 1], svalue[MAX_STR_BASE_T + 1];
	address_t addr;
	base_t value;
	int i;	
	struct reg_s *reg = &register_buf;
	dbg("Here;");
	switch(index) {
		case 0: /* Address */
			for(i=0; i < MAX_STR_ADDR_T && ok(b, i + oval); ++i)
				saddr[i] = at(b, oval + i);		
			saddr[MAX_STR_ADDR_T] = '\0';
	
			if( string_to_addr(saddr, &addr) == -1 ) {		
				req_err(arg->req,401,"Bad reg address request");
				memset(reg, 0, sizeof(struct reg_s));
				return -1;
			}
			reg->addr = addr;
			return 0;
		case 1: /* Value */
			for(i=0; i < MAX_STR_BASE_T && ok(b, i + oval); ++i)
				svalue[i] = at(b, oval + i);		
			svalue[MAX_STR_BASE_T] = '\0';
			if( string_to_base(svalue, &value) == -1 ) {		
				req_err(arg->req,401,"Bad reg address request");
				memset(reg, 0, sizeof(struct reg_s));
				return -1;
			}	
			reg->value = value;
			reg->ok = 1;
			return 0;
		default: /* Should not happen */
			req_err(arg->req,401,"Bad reg write request");
			memset(reg, 0, sizeof(struct reg_s));
			return -1;						
	}
	
	return 0;
}
int execute_write_reg(struct post_method *self, struct http_request *req)
{
	struct reg_s *reg = &register_buf;
	
	if(!reg->ok) {
		req_err(req,401,"Bad reg write request");
		memset(reg, 0, sizeof(struct reg_s));
		return -1;
	}
	
	dbg("Here;");
	write_register(reg->addr, reg->value);
	dbg("Here;");
	memset(reg, 0, sizeof(struct reg_s));
	
	if(reply_request(req, string_static("\"success\""))) {
		req_internal(req,"read_request_body()");
		return -1;		
	}
	
	
	return 0;
}
/*-----------------------------------------------------------*/
/* Memeory I/O */

 
struct mem_s {
	address_t addr;
	address_t length;
	base_t data[MAX_MEM_BUF_LEN];	

	int ok;
} memory_buf = {};



/**
 *    [ "address", "count" ]
 */



int cb_read_mem(xjson *b, int index, offset oval, int vlen,  void *varg)
{
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	char saddr[MAX_STR_ADDR_T + 1];
	address_t addr;
	int i;	
	struct mem_s *mem = &memory_buf;

	switch(index) {
		case 0: /* Address */
			for(i=0; i < MAX_STR_ADDR_T && ok(b, i + oval); ++i)
				saddr[i] = at(b, oval + i);		
			saddr[MAX_STR_ADDR_T] = '\0';
	
			if( string_to_addr(saddr, &addr) == -1 ) {		
				req_err(arg->req,401,"Bad reg address request");
				return -1;
			}
			mem->addr = addr;

			return 0;
		case 1:

			for(i=0; i < MAX_STR_ADDR_T && ok(b, i + oval); ++i)
				saddr[i] = at(b, oval + i);		
			saddr[MAX_STR_ADDR_T] = '\0';
	
			if( string_to_addr(saddr, &addr) == -1 ) {		
				req_err(arg->req,401,"Bad reg address request");
				return -1;
			}
			mem->length = addr;
			mem->ok = 1;
			return 0;

		default: /* Should not happen */
			req_err(arg->req,401,"Bad reg write request");
			return -1;						
	}
	
	
}	
int __send_data(struct http_request *req, base_t *buf, address_t len, int first)
{	
	int i;
	for(i = 0; i < len; i++ ) {
		char sbase[MAX_STR_BASE_T + 1];
		base_t num = buf[i];
		

		if(likely(!( first == 1 && i == 0 ))) {
			if(netconn_printf(req->netconn, ", ")) {
				req_internal(req, "netconn_printf(',')");
				return -1;
			}
		}
		ssize_t len = base_to_string(num, sbase);
		if( len == -1) {
			req_internal(req, "base_to_string()");
			return -1;
		}
		
		if(netconn_printf(req->netconn, "%.*s",len, sbase)) {
			req_internal(req, "netconn_printf(',')");
			return -1;
		}
		
	}
	return 0;

}
int _read_and_send_data(struct http_request *req, struct mem_s *mem)
{
	address_t i, length;
	address_t start = mem->addr;
	address_t end = mem->addr + mem->length;
	
	
	if(netconn_printf(req->netconn, "[ ")) {
		req_internal(req, "netconn_printf('[')");
		return -1;
	}
	
	for(i = start; i < end ; i += MAX_MEM_BUF_LEN ) {
		length = MIN(MAX_MEM_BUF_LEN, end-i);
		read_memory(i, mem->data, length );
		
		dprintf("length=%d, i=%d, MAX_MEM_BUF_LEN=%d, end-start=%d\n", length, i, MAX_MEM_BUF_LEN, end-i);
		
		if(__send_data(req, mem->data, length, i == start)) {
			req_internal(req, "__send_data()");
			return -1;		
		}
		memset(mem->data, 0, sizeof(base_t) * MAX_MEM_BUF_LEN);
	}
	if(netconn_printf(req->netconn, " ]")) {
		req_internal(req, "netconn_printf(']')");
		return -1;
	}
	return 0;
}
int execute_read_mem(struct post_method *self, struct http_request *req)
{
	struct mem_s *mem = &memory_buf;
	
	if(!mem->ok) {
		req_err(req,401,"Bad reg write request");
		goto err_exit;
	}
	if(_reply_head(req) == -1) {
		req_internal(req,"_reply_head()");
		goto err_exit;
	}
	
	
	if(netconn_printf(req->netconn, "{\"result\":")) {
		req_internal(req, "netconn_printf({\"result\":)");
		goto err_exit;
	}
	
	if(_read_and_send_data(req, mem)) {
		req_internal(req,"_read_and_send_data()");
		goto err_exit;
	}
	
	if(netconn_printf(req->netconn, "}")) {
		req_internal(req, "netconn_printf(})");
		goto err_exit;
	}
	memset(mem, 0, sizeof(struct mem_s));
	return 0;
	
err_exit:	
	memset(mem, 0, sizeof(struct mem_s));
	return -1;
}
/*-----------------------------------------------------------*/
/**
 *		 /----data----\
 *    [ address, [ 1, 2, 3, 4 ] ]
 */
 


void __push_data(struct mem_s *buf, base_t data)
{
	if(buf->length < MAX_MEM_BUF_LEN) {
		buf->data[buf->length++] = data;
		return;
	}
	
	dbg("Write %d", buf->length);
	write_memory(buf->addr, buf->data, buf->length);
	
	buf->addr += buf->length;

	memset(buf->data, 0, sizeof(base_t) * MAX_MEM_BUF_LEN);
	
	buf->length = 1;
	buf->data[0] = data;
}
int __cb_wmem_data(xjson *b, int index, offset oval, int vlen,  void *varg)
{
	struct mem_s *mem = (struct mem_s *) varg;
	char  svalue[MAX_STR_BASE_T + 1];
	base_t value;
	int i;
	
	dbg("Index %d", index);
	
	for(i=0; i < MAX_STR_BASE_T && ok(b, i + oval) && i < vlen; ++i)
		svalue[i] = at(b, oval + i);		
	svalue[i] = '\0';
	dbg("Value %s", svalue);
	if( string_to_base(svalue, &value) == -1 ) {	
		dbg("FAIL");
		return -1;
	}
	__push_data(mem, value);
	
	mem->ok = 1;
	return 0;
}

int cb_write_mem(xjson *b, int index, offset oval, int vlen,  void *varg)
{
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	char saddr[MAX_STR_ADDR_T + 1];
	address_t addr;
	int i;	
	struct mem_s *mem = &memory_buf;

	switch(index) {
		case 0: /* Address */
			for(i=0; i < MAX_STR_ADDR_T && ok(b, i + oval); ++i)
				saddr[i] = at(b, oval + i);		
			saddr[MAX_STR_ADDR_T] = '\0';
	
			if( string_to_addr(saddr, &addr) == -1 ) {		
				req_err(arg->req,401,"Bad mem address request");
				memset(mem, 0, sizeof(struct mem_s));	
				return -1;
			}
			mem->addr = addr;
			mem->ok = 1;
			return 0;
		case 1:
			if(JSON_decode_array( b, oval, __cb_wmem_data, (void *) mem ) == 0)
				break;
				
					 /*! Fall through if error !*/
			 
		default: /* Should not happen */
			req_err(arg->req,401,"Bad mem write request");
			memset(mem, 0, sizeof(struct mem_s));	
			return -1;						
	}
	return 0;
	
	
}
int execute_write_mem(struct post_method *self, struct http_request *req)
{
	struct mem_s *buf = &memory_buf;
	
	if(!buf->ok) {
		req_err(req,401,"Bad reg write request");
		memset(buf, 0, sizeof(struct mem_s));	
		return -1;
	}
	
	if(buf->length) {

		write_memory(buf->addr, buf->data, buf->length);

		memset(buf, 0, sizeof(struct mem_s));	
	}	

	if(reply_request(req, string_static("\"success\""))) {
		req_internal(req,"read_request_body()");
		memset(buf, 0, sizeof(struct mem_s));	
		return -1;		
	}


	return 0;
}

/*-----------------------------------------------------------*/
int cb_first_parse (xjson *b, offset oname, int nlen, offset oval, int vlen, void *varg)
{	
	struct parse_cb_arg *arg = (struct parse_cb_arg*) varg;
	// int rcmp = pbuf_memcmp((struct pbuf *)b, oname, "\"req\"", 5);  


   	if(JSON_decode(b, oval, arg->m->parse_array ,arg->m->parse_object, arg ) == -1) 
		return -1;				
	return 0;
}

struct post_method post_method_lookup[] = {	
	{ 			
		.parse_object = cb_json_print_obj,
		.parse_array = cb_json_print_arr,
		.data = (void *) 0,
		
				
		.name = "data.print",
	},	
	{ 			
		.parse_array = cb_read_reg,
		.execute = execute_read_reg,
		.name = "reg.read",
	},
	{ 			
		.parse_array = cb_write_reg,
		.execute = execute_write_reg,
		.name = "reag.write",
	},	
	{ 		
		.parse_array = cb_read_mem,	
		.execute = execute_read_mem,
		.name = "mem.read",
	},
	{ 				
		.parse_array = cb_write_mem,	
		.execute = execute_write_mem,
		.name = "mem.write",
	},
};

int execute_post_method(struct http_request *req, struct netbuf *body, int skip)
{
	struct post_method *m = req->method;
	struct parse_cb_arg arg = { .m = m, .req = req };
	int rv = 0;
	
	if(take_api()) {
		req_err(req, 505, "failed to take_api()");
		return -1;
	}
	
	if(m->parse_object || m->parse_array) {
		if(JSON_decode_object( body, skip, cb_first_parse, (void *) &arg ) == -1) {
			req_err(req, 401, "filed to parse json object");			
			rv = -1;		
			goto ret;
		}
	}
	if(!m->execute) 
		goto ret;
			
	rv = m->execute(m, req);
ret:
	release_api();
	return rv; 
}

