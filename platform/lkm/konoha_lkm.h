#ifndef KONOHA_LKM_H_
#define KONOHA_LKM_H_

#ifndef __KERNEL__
#error
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <asm/uaccess.h>

#define KNH_EXT_QSORT  1
#define KNH_EXT_SETJMP 1

/* stdint.h */
#ifndef _STDINT_H
#define _STDINT_H
typedef long intptr_t;
#endif

/* /usr/include/inttypes.h */
#define PRIdPTR "d"
#define PRIuPTR "u"

typedef intptr_t FILE;

#define TODO_LKM
#define getenv(a) NULL
#define stdin  ((FILE*)NULL)
#define stdout KERN_INFO
#define stderr KERN_ALERT

//#define malloc(x) kmalloc(x,GFP_KERNEL)

#define calloc(x,y) kcalloc(x,y,GFP_KERNEL)
#define realloc(x,y) krealloc(x,y,GFP_KERNEL)

static inline void *malloc(size_t size)
{
	return kmalloc(size,GFP_KERNEL);
}

static inline void free(void *p)
{
	kfree(p);
}

#define strtoll(x,y,z) kstrtoll(x,z,y)
#define bzero(x,y) memset(x,0x00,y)
//#define fopen(a,b) NULL
static inline FILE *fopen(const char *a,const char *b)
{
	(void)a;(void)b;
	return NULL;
}

//#define fclose(fp)
static inline int fclose(FILE *fp)
{
	return 0;
}
#define dlopen(a,b) NULL
#define dlsym(a,b) NULL
//#define realpath(path,buf) NULL
static inline char *realpath(const char *a,char *b)
{
	(void)a;(void)b;
	return NULL;
}
#define fprintf(out,fmt, arg...) printk(KERN_ALERT fmt , ##arg )
#define vfprintf(out,fmt, arg...) vprintk(fmt , arg )
#define fputs(prompt, fp) 
//#define fgetc(fp) (-1)
static inline int fgetc(FILE *fp)
{
	return -1;
}
#define EOF -1
#define fflush(x)
#define exit(i)  printk(KERN_EMERG "KONOHA_exit!!!")
#define assert(x) BUG_ON(!(x))
#define abort() BUG_ON(1)

/* setjmp.S */
#if defined(__i386__)
/* return_addr, ebx, esp, ebp, esi, edi */
#define JMP_BUFFSIZE 6

#elif defined(__x86_64__)
/* return_addr, rbx, rsp, rbp, r12, r13, r14, r15 */
#define JMP_BUFFSIZE 8
#endif

typedef struct {
	unsigned long __jmp_buf[JMP_BUFFSIZE];
} jmp_buf[1];

//int setjmp(jmp_buf env);
//int longjmp(jmp_buf env, int val);

/* ../../src/ext/qsort.c */
void qsort (void *const pbase, size_t total_elems, size_t size,
				int (*cmp)(const void*,const void*));

/* ../../src/ext/strerror.c */
//char* strerror(int errno);
/* ------------------------------------------------------------------------ */

#endif /* KONOHA_LKM_H_ */
