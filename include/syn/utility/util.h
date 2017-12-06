/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SIS_UTIL_H_
#define _SIS_UTIL_H_


#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <strings.h>
#include <cassert>
#include <stdarg.h>
#include <stdint.h>

namespace SYN_SIS {

#define EXTERN extern

#define NIL(type)		((type *) 0)

#define ALLOC(type, num)	\
    ((type *) malloc((long) sizeof(type) * (long) (num)))
#define REALLOC(type, obj, num)	\
    ((type *) realloc((char *) (obj), (long) sizeof(type) * (long) (num)))
#define FREE(obj)		\
    ((obj) ? (free((void *) (obj)), (obj) = 0) : 0)


/* These arguably do NOT belong in util.h */
#ifndef ABS
#define ABS(a)			((a) < 0 ? -(a) : (a))
#endif
#ifndef MAX
#define MAX(a,b)		((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)		((a) < (b) ? (a) : (b))
#endif

#define ARGS(args)  args

EXTERN void util_print_cpu_stats ARGS((FILE *));
EXTERN long util_cpu_time ARGS((void));
EXTERN void util_getopt_reset ARGS((void));
EXTERN int util_getopt ARGS((int, char **, char *));
EXTERN char *util_path_search ARGS((char *));
EXTERN char *util_file_search ARGS((char *, char *, char *));
EXTERN int util_pipefork ARGS((char **, FILE **, FILE **, int *));
EXTERN char *util_print_time ARGS((long));
EXTERN int util_save_image ARGS((char *, char *));
EXTERN char *util_strsav ARGS((char *));
EXTERN int util_do_nothing ARGS((void));
EXTERN char *util_tilde_expand ARGS((char *));
EXTERN char *util_tempnam ARGS((char *, char *));
EXTERN FILE *util_tmpfile ARGS((void));

#define ptime()         util_cpu_time()
#define print_time(t)   util_print_time(t)
//#define fail(why) {                                               
//    (void) fprintf(stderr, "Fatal error: file %s, line %d\n%s\n",	
//                   __FILE__, __LINE__, why);                      
//    (void) fflush(stdout);                                        
//    abort();                                                      
//  }
  
}
#endif //_SIS_UTIL_H_
