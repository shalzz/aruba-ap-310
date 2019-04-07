#ifndef _ARUBA_TYPES_H_
#define _ARUBA_TYPES_H_

#ifndef _ARUBA_LOCAL_TYPES
/*
 * In the header files, abbreviated type names are defined only within the 
 * scope of the kernel space. This causes a lot of issues with the 2.6 kernel 
 * when Aruba code include a linux header files directly. Additionally, there 
 * are also header files such as "net/ppp_defs.h" that end up including kernel 
 * headers, and we run into the same problem. What a bloody mess. 
 *
 * For now, we define these types locally. Eventually we should move away from
 * kernel space includes to avoid these issues. 
 */
typedef __signed char s8;
typedef unsigned char u8;

typedef __signed short s16;
typedef unsigned short u16;

typedef __signed int s32;
typedef unsigned int u32;

typedef __signed__ long long s64;
typedef unsigned long long u64;
#define _ARUBA_LOCAL_TYPES
#endif /* _ARUBA_LOCAL_TYPES */

/* Including linux/ext2_fs.h is another fine mess. */
#ifndef EXT2_SUPER_MAGIC
#define EXT2_SUPER_MAGIC      0xEF53
#endif 

typedef  u16  instance_t;

/* handle to ptr conversion macros */
#define handle_to_ptr(type, handle)    (type##_t *) handle
#define ptr_to_handle(type, ptr)       (type##_handle) ptr

#endif /* _ARUBA_TYPES_H_ */
