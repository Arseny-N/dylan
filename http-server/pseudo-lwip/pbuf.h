/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef __LWIP_PBUF_H__
#define __LWIP_PBUF_H__

//#include "lwip/opt.h"
//#include "lwip/err.h"

typedef unsigned char 	u8_t;
typedef unsigned short 	u16_t;
typedef unsigned int 	u32_t;

typedef signed char 	s8_t;
typedef signed short 	s16_t;
typedef signed int 	s32_t;

typedef unsigned long mem_ptr_t;
typedef u16_t err_t;


#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM        -1    /* Out of memory error.     */
#define ERR_BUF        -2    /* Buffer error.            */
#define ERR_TIMEOUT    -3    /* Timeout.                 */
#define ERR_RTE        -4    /* Routing problem.         */
#define ERR_INPROGRESS -5    /* Operation in progress    */
#define ERR_VAL        -6    /* Illegal value.           */
#define ERR_WOULDBLOCK -7    /* Operation would block.   */
#define ERR_USE        -8    /* Address in use.          */
#define ERR_ISCONN     -9    /* Already connected.       */

#define ERR_IS_FATAL(e) ((e) < ERR_ISCONN)

#define ERR_ABRT       -10   /* Connection aborted.      */
#define ERR_RST        -11   /* Connection reset.        */
#define ERR_CLSD       -12   /* Connection closed.       */
#define ERR_CONN       -13   /* Not connected.           */

#define ERR_ARG        -14   /* Illegal argument.        */

#define ERR_IF         -15   /* Low-level netif error    */


#define MEM_ALIGNMENT  1
#define PBUF_LINK_HLEN 14
#define LWIP_MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#define LWIP_MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))\

#define MEMCPY(dst,src,len)             memcpy(dst,src,len)


/** lower two bits indicate debug level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define LWIP_DBG_LEVEL_ALL     0x00
#define LWIP_DBG_LEVEL_OFF     LWIP_DBG_LEVEL_ALL /* compatibility define only */
#define LWIP_DBG_LEVEL_WARNING 0x01 /* bad checksums, dropped packets, ... */
#define LWIP_DBG_LEVEL_SERIOUS 0x02 /* memory allocation failures, ... */
#define LWIP_DBG_LEVEL_SEVERE  0x03
#define LWIP_DBG_MASK_LEVEL    0x03

/** flag for LWIP_DEBUGF to enable that debug message */
#define LWIP_DBG_ON            0x80U
/** flag for LWIP_DEBUGF to disable that debug message */
#define LWIP_DBG_OFF           0x00U

/** flag for LWIP_DEBUGF indicating a tracing message (to follow program flow) */
#define LWIP_DBG_TRACE         0x40U
/** flag for LWIP_DEBUGF indicating a state debug message (to follow module states) */
#define LWIP_DBG_STATE         0x20U
/** flag for LWIP_DEBUGF indicating newly added code, not thoroughly tested yet */
#define LWIP_DBG_FRESH         0x10U
/** flag for LWIP_DEBUGF to halt after printing this debug message */
#define LWIP_DBG_HALT          0x08U

#ifndef error
 #define error(fmt, args...) 		fprintf(stderr, __FILE__ ":%s:%d ERROR: " fmt "\n", __func__, __LINE__, ##args)
#endif

#define LWIP_PLATFORM_ASSERT(m) do{error(m);}while(0);

#ifndef LWIP_NOASSERT
 #define LWIP_ASSERT(message, assertion) do { if(!(assertion)) \
  LWIP_PLATFORM_ASSERT(message); } while(0)
#else  /* LWIP_NOASSERT */
 #define LWIP_ASSERT(message, assertion) 
#endif /* LWIP_NOASSERT */

/** if "expression" isn't true, then print "message" and execute "handler" expression */
#ifndef LWIP_ERROR
 #define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  LWIP_PLATFORM_ASSERT(message); handler;}} while(0)
#endif /* LWIP_ERROR */

#ifdef LWIP_DEBUG
/** print debug message only if debug message type is enabled...
 *  AND is of correct type AND is at least LWIP_DBG_LEVEL
 */
 #define LWIP_DEBUGF(debug, message) do { \
                               if ( \
                                   ((debug) & LWIP_DBG_ON) && \
                                   ((debug) & LWIP_DBG_TYPES_ON) && \
                                   ((s16_t)((debug) & LWIP_DBG_MASK_LEVEL) >= LWIP_DBG_MIN_LEVEL)) { \
                                 LWIP_PLATFORM_DIAG(message); \
                                 if ((debug) & LWIP_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)

#else  /* LWIP_DEBUG */
 #define LWIP_DEBUGF(debug, message) 
#endif /* LWIP_DEBUG */



#ifdef __cplusplus
extern "C" {
#endif

/** Currently, the pbuf_custom code is only needed for one specific configuration
 * of IP_FRAG */
#define LWIP_SUPPORT_CUSTOM_PBUF (IP_FRAG && !IP_FRAG_USES_STATIC_BUF && !LWIP_NETIF_TX_SINGLE_PBUF)

#define PBUF_TRANSPORT_HLEN 20
#define PBUF_IP_HLEN        20

typedef enum {
  PBUF_TRANSPORT,
  PBUF_IP,
  PBUF_LINK,
  PBUF_RAW
} pbuf_layer;

typedef enum {
  PBUF_RAM, /* pbuf data is stored in RAM */
  PBUF_ROM, /* pbuf data is stored in ROM */
  PBUF_REF, /* pbuf comes from the pbuf pool */
  PBUF_POOL /* pbuf payload refers to RAM */
} pbuf_type;


/** indicates this packet's data should be immediately passed to the application */
#define PBUF_FLAG_PUSH      0x01U
/** indicates this is a custom pbuf: pbuf_free and pbuf_header handle such a
    a pbuf differently */
#define PBUF_FLAG_IS_CUSTOM 0x02U
/** indicates this pbuf is UDP multicast to be looped back */
#define PBUF_FLAG_MCASTLOOP 0x04U
/** indicates this pbuf was received as link-level broadcast */
#define PBUF_FLAG_LLBCAST   0x08U
/** indicates this pbuf was received as link-level multicast */
#define PBUF_FLAG_LLMCAST   0x10U
/** indicates this pbuf includes a TCP FIN flag */
#define PBUF_FLAG_TCP_FIN   0x20U

struct pbuf {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  u16_t tot_len;

  /** length of this buffer */
  u16_t len;

  /** pbuf_type as u8_t instead of enum to save space */
  u8_t /*pbuf_type*/ type;

  /** misc flags */
  u8_t flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  u16_t ref;
};

#if LWIP_SUPPORT_CUSTOM_PBUF
/** Prototype for a function to free a custom pbuf */
typedef void (*pbuf_free_custom_fn)(struct pbuf *p);

/** A custom pbuf: like a pbuf, but following a function pointer to free it. */
struct pbuf_custom {
  /** The actual pbuf */
  struct pbuf pbuf;
  /** This function is called when pbuf_free deallocates this pbuf(_custom) */
  pbuf_free_custom_fn custom_free_function;
};
#endif /* LWIP_SUPPORT_CUSTOM_PBUF */

#if LWIP_TCP && TCP_QUEUE_OOSEQ
/** Define this to 0 to prevent freeing ooseq pbufs when the PBUF_POOL is empty */
#ifndef PBUF_POOL_FREE_OOSEQ
#define PBUF_POOL_FREE_OOSEQ 1
#endif /* PBUF_POOL_FREE_OOSEQ */
#if NO_SYS && PBUF_POOL_FREE_OOSEQ
extern volatile u8_t pbuf_free_ooseq_pending;
void pbuf_free_ooseq();
/** When not using sys_check_timeouts(), call PBUF_CHECK_FREE_OOSEQ()
    at regular intervals from main level to check if ooseq pbufs need to be
    freed! */
#define PBUF_CHECK_FREE_OOSEQ() do { if(pbuf_free_ooseq_pending) { \
  /* pbuf_alloc() reported PBUF_POOL to be empty -> try to free some \
     ooseq queued pbufs now */ \
  pbuf_free_ooseq(); }}while(0)
#endif /* NO_SYS && PBUF_POOL_FREE_OOSEQ*/
#endif /* LWIP_TCP && TCP_QUEUE_OOSEQ */

/* Initializes the pbuf module. This call is empty for now, but may not be in future. */
#define pbuf_init()

struct pbuf *pbuf_alloc(pbuf_layer l, u16_t length, pbuf_type type);
#if LWIP_SUPPORT_CUSTOM_PBUF 
struct pbuf *pbuf_alloced_custom(pbuf_layer l, u16_t length, pbuf_type type,
                                 struct pbuf_custom *p, void *payload_mem,
                                 u16_t payload_mem_len);
#endif /* LWIP_SUPPORT_CUSTOM_PBUF */
void pbuf_realloc(struct pbuf *p, u16_t size); 
u8_t pbuf_header(struct pbuf *p, s16_t header_size);
void pbuf_ref(struct pbuf *p);
u8_t pbuf_free(struct pbuf *p);
u8_t pbuf_clen(struct pbuf *p);  
void pbuf_cat(struct pbuf *head, struct pbuf *tail);
void pbuf_chain(struct pbuf *head, struct pbuf *tail);
struct pbuf *pbuf_dechain(struct pbuf *p);
err_t pbuf_copy(struct pbuf *p_to, struct pbuf *p_from);
u16_t pbuf_copy_partial(struct pbuf *p, void *dataptr, u16_t len, u16_t offset);
err_t pbuf_take(struct pbuf *buf, const void *dataptr, u16_t len);
struct pbuf *pbuf_coalesce(struct pbuf *p, pbuf_layer layer);
#if LWIP_CHECKSUM_ON_COPY
err_t pbuf_fill_chksum(struct pbuf *p, u16_t start_offset, const void *dataptr,
                       u16_t len, u16_t *chksum);
#endif /* LWIP_CHECKSUM_ON_COPY */

u8_t pbuf_get_at(struct pbuf* p, u16_t offset);
u16_t pbuf_memcmp(struct pbuf* p, u16_t offset, const void* s2, u16_t n);
u16_t pbuf_memfind(struct pbuf* p, const void* mem, u16_t mem_len, u16_t start_offset);
u16_t pbuf_strstr(struct pbuf* p, const char* substr);

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_PBUF_H__ */
