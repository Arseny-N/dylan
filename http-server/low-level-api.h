
#ifndef __LOWLEVEL_API_H__
#define __LOWLEVEL_API_H__
#include <sys/types.h>
#include <stdint.h>


typedef uint32_t address_t; 
typedef uint32_t base_t;

/**
 *  MAX_UINT32 = 2^32 = 4 XXX XXX XXX <- 10 digits :-P
 *  
 */
#define MAX_STR_ADDR_T  16
#define MAX_STR_BASE_T  16

/**
 * MAX_MEM_BUF_LEN - With buffers of this size mem data is sent from memory to host.
 * TODO: move somewere else ?
 * See usege in:
 * 	- post_methods.c
 *		- _read_and_send_data()
 *		- struct mem_s
 */

#define MAX_MEM_BUF_LEN   16

typedef void (*write_register_t)  (address_t addr, base_t  value);
typedef void (*read_register_t)   (address_t addr, base_t *value);


typedef void (*write_memory_t)  (address_t addr, base_t *value, size_t len);
typedef void (*read_memory_t)   (address_t addr, base_t *value, size_t len);

typedef int  (*take_func_t)   (void);
typedef void (*release_func_t)(void);
typedef int  (*open_func_t)   (void);
typedef void (*close_func_t)  (void);

struct lowlevel_api {
	
	take_func_t    take; 
	release_func_t release;
	
	open_func_t    open; 
	close_func_t  close;  
	
	write_register_t reg_write;
	read_register_t  reg_read;
	
	write_memory_t mem_write;
	read_memory_t  mem_read;

	ssize_t (*base_to_string)(base_t num, char string[MAX_STR_BASE_T + 1]);
	int 	(*string_to_base)(char string[MAX_STR_BASE_T + 1], base_t *num);

	int (*string_to_addr)(char saddr[MAX_STR_ADDR_T + 1], address_t *addr);
	

	char *name;	
};


ssize_t base_to_string(base_t num, char string[MAX_STR_BASE_T + 1]);
int 	string_to_base(char string[MAX_STR_BASE_T + 1], base_t *num);
int 	string_to_addr(char address[MAX_STR_ADDR_T + 1],address_t *);


void   write_register(address_t addr, base_t value);
void read_register (address_t addr, base_t *value);


void write_memory  (address_t addr, base_t *value,  size_t len);
void read_memory   (address_t addr, base_t *value,  size_t len);

int  take_api   (void);
void release_api   (void);

int  open_api   (void);
void close_api   (void);

extern struct lowlevel_api *API;


#endif /* __LOWLEVEL_API_H__ */
