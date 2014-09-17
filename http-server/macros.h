#ifndef __MACROS__H__ 
#define __MACROS__H__ 


#define DEBUG
#define JSON_ERRORS  
#define JSONX_LWIP



#define ARR_LEN(a) (sizeof(a)/sizeof(a[0]))
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

#ifdef DEBUG

#include <stdio.h>
struct debug_context {
	char *file;
	char *function;
	int   line;
};

 #define save_debug_context(d_ptr) \
  do {\
  	(d_ptr)->file = __FILE__; \
  	(d_ptr)->line = __LINE__;\
	(d_ptr)->function = (char *)__func__;\
  }while(0)
	



 #define dprintf(fmt, args...) 		printf( fmt, ##args)
 #define dbg(fmt, args...) 		printf( "DBG:" __FILE__ ":%s:%d: " fmt "\n", __func__, __LINE__, ##args)
 #define snprintf_dbg_ctx(buf, size, ctx)  snprintf(buf, size, "%s:%s:%ld", ctx->file, ctx->function, ctx->line)
 #define snprintf_req_dctx(buf, size, req) snprintf_dbg_ctx(buf, size, &(req->dbg_ctx))
 
 #define __req_err_force(req, code, msg) 		do{ 	\
		(req)->http_code = (unsigned int) (code); 	\
		(req)->header_msg = msg;			\
		(req)->req_type = HTTP_HEADER_ERR;		\
		save_debug_context(&((req)->dbg_ctx));	 	\
  }while(0)
  
#else

 #define __req_err_force(req, code, msg) 		do{ 	\
	(req)->http_code = (unsigned int) (code); 		\
	(req)->header_msg = msg;				\
	(req)->req_type = HTTP_HEADER_ERR;			\
 }while(0)
  
 #define dprintf(fmt, args...) 		
 #define dbg(fmt, args...)
 #define snprintf_dbg_ctx(buf, size, ctx)
 #define snprintf_req_dctx(buf, size, req) 
 
#endif /* DEBUG */

#define req_err(req, code, msg) 		do{ 	\
	error("(%d) %s", code, msg);			\
	if((req)->http_code == 0 || (req)->http_code == 200)  {			\
		__req_err_force(req, code, msg); 	\
	}						\
}while(0)

#define req_internal(req, msg) req_err(req, (500), msg)



#define likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)


#define HTTP_HEADER_NO      0
#define HTTP_HEADER_GET     1
#define HTTP_HEADER_POST    2
#define HTTP_HEADER_ERR     255


#endif /* __MACROS__H__ */
