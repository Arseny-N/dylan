#include <string.h>
#include <errno.h>

#include "macros.h" 
#include "low-level-api.h"

#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

ssize_t __base_to_string(base_t num, char string[MAX_STR_BASE_T + 1])
{
	return snprintf(string, MAX_STR_BASE_T,"%lld", (long long)num);	
}
int __string_to_base(char string[MAX_STR_BASE_T + 1], base_t *num)
{
	char *dummy;
	int saved_errno = errno;
	errno = 0;
	base_t n = strtol(string, &dummy, 10);
	if(errno || dummy == string )
		return -1;
	errno = saved_errno;
	*num = n;
	return 0;	
}

int __string_to_addr(char string[MAX_STR_ADDR_T + 1], address_t *addr)
{
	char *dummy;
	int saved_errno = errno;
	errno = 0;
	address_t n = strtol(string, &dummy, 10);
	if(errno || dummy == string )
		return -1;
	errno = saved_errno;
	*addr = n;
	return 0;	
}

#define __define_conv_func(r_type, fname, a1, a2, v1, v2) \
r_type fname (a1, a2) { \
	if(API && API -> fname )	return (r_type) API-> fname (v1, v2) ;\
	return (r_type) __##fname (v1, v2) ;\
}

__define_conv_func(ssize_t, base_to_string, base_t num, char string[MAX_STR_BASE_T + 1], num, string );
__define_conv_func(int, string_to_base, char string[MAX_STR_BASE_T + 1], base_t *num, string, num );
__define_conv_func(int, string_to_addr, char string[MAX_STR_ADDR_T + 1], address_t *addr, string, addr );

#undef __define_conv_func

/*--------------------------------------------------------------*/
/* Dummy Api */
#define DUMMY_MEM_SIZE 128
base_t registers[] = {0x1, 0x44, 0x15, 0x16};
base_t memory[DUMMY_MEM_SIZE] = { [0 ... DUMMY_MEM_SIZE-1] = (base_t) 404 };

void print_bmem(char *prefix, char *fmt, void *mem, size_t len, unsigned int line_length)
{
	if(prefix)
		printf("%s", prefix);
	int i;
	base_t *bytes = (base_t *) mem;
	for(i = 0; i< len; ++i) {
		if( !(i % line_length) && i )
			printf("\n");
		printf(fmt, bytes[i]);
	}
	printf("\n");
}


void dummy_read_register(address_t addr, base_t *val)
{	
	if(val)	 {
		if(addr >= 0 && addr < ARR_LEN(registers))
			*val = registers[addr];
		else
			*val =  0x0;
	}
}
void dummy_write_register(address_t addr, base_t value)
{	
	if(addr >= 0 && addr < ARR_LEN(registers))
		registers[addr] = value;
}
void dummy_write_memory (address_t addr, base_t *value,  size_t len)
{
	int mlen = ARR_LEN(memory);
	if(addr >= 0 && addr < mlen) {
		int j, min = MIN(len, mlen-addr);
		for(j = 0; j < min; ++j ) 
			memory[addr + j] = value[j];
			
		dprintf("addr=%d,len=%d,min=%d,i=%d\n", addr,len, min, j);
		print_bmem(" write-mem ", "%2d ", value, MIN(len, DUMMY_MEM_SIZE-addr), 40);		
	}
	
	
}
void dummy_read_memory (address_t addr, base_t *value,  size_t len)
{
	address_t mlen = ARR_LEN(memory);
	address_t end = addr + len;
	int j, min = MIN(len, mlen-addr);
	
	if(addr >= 0 && addr < mlen) {

		for(j = 0; j < min; ++j ) 
			value[j] = memory[addr + j];		
		print_bmem(" read-mem ", "%2d ", value, min, 20);
	}
	dprintf("addr=%d,len=%d,min=%d,i=%d,mlen=%ld,end=%d \n", addr,len, min, j,(long) mlen,end);	
}

struct lowlevel_api dummy_api = {
	
	.reg_write = dummy_write_register,
	.reg_read = dummy_read_register,
	
	.mem_write = dummy_write_memory,
	.mem_read = dummy_read_memory,

	.name = "dummy api", /* ??? */
};

struct lowlevel_api *API =  &dummy_api;

#define __define_api_func( fname, stname,  d_args,  c_args) \
void fname d_args { \
	if(API && API -> stname ) API-> stname c_args ;\
}


__define_api_func( read_register, reg_read, (address_t addr, base_t *value), (addr, value) );
__define_api_func( write_register,reg_write, (address_t addr, base_t  value), (addr, value) );

__define_api_func( write_memory,mem_write, (address_t addr, base_t *value,  size_t len), (addr, value, len) );
__define_api_func( read_memory, mem_read,  (address_t addr, base_t *value,  size_t len), (addr, value, len) );


__define_api_func( release_api,release, (void), () );
__define_api_func( close_api,close, (void), () );

int open_api(void) 
{
	if(API && API->open ) return API->open();
	return 0;
}
int take_api(void) 
{
	if(API && API->take ) return API->take();
	return 0;
}

#undef __define_api_func


