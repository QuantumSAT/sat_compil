/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SIS_FAST_EXTRACT_INT_H_
#define _SIS_FAST_EXTRACT_INT_H_

#include "syn/utility/sis_list.h"
#include "syn/utility/sis_heap.h"
#include "syn/utility/sis_array.h"
#include "syn/utility/sparse.h"
#include "extract.h"
#include "extract_int.h"

namespace SYN_SIS {
#if 0
/* some definitions in "extract.h" */
/* Some definitions for making the structures in fast_extract exportable */

#define SHORT short int
#define UNSIGNED unsigned char

/*  Define the data structure a double cube diviosr */
typedef struct double_cube_divisor_struct ddivisor_t;
struct double_cube_divisor_struct {
    sm_row *cube1;          /* the first cube of the double-cube divisor */
    sm_row *cube2;          /* the second cube of the double-cube divisor */
    lsHandle handle;        /* lsHandle to double-cube divisor set */
    lsHandle sthandle;      /* lsHandle to corresponoding searching table */
    lsList div_index;       /* indicate where the divisor comes from */
    SHORT weight;           /* the weight of double-cube divisor */
    UNSIGNED dtype;         /* the type of double-cube divisor */
    UNSIGNED weight_status; /* indicate the weight can be changed or not */
    UNSIGNED status;        /* indicate whether the divisor is changed or not */
    UNSIGNED level;         /* indicates the level of 2-cube kernels */
};


#define fx_get_div_handle(p)    ((p)->handle)
/* end of extract.h */
#endif
/* some definitions from SIS network.h */
#define lsForeachItem(list, gen, data)				\
    for(gen = lsStart(list); 					\
	(lsNext(gen, (lsGeneric *) &data, LS_NH) == LS_OK) 	\
	    || ((void) lsFinish(gen), 0); )
/* end of SIS network.h */

/*
 *  Definitions local to package 'fast_extract' go here
 */

/* SHORT and UNSIGNED are now defined in extract.h */

/*  Define types of double cube divisors */
#define D112     	0
#define D222     	1
#define D223     	2
#define D224     	3
#define Dother   	4

/*  Define different status for weights in divisors */
#define OLD      	0
#define NEW      	1
#define CHANGED  	2

/*  Define different status of doube-cube divisor after extracting 
 *  single-cube divisor
 */
#define NONCHECK    	0
#define INBASE      	1
#define INBETWEEN   	2
#define INDIVISOR   	3

#define HANDLE(p)    ((p)->handle)
#define WEIGHT(p)    ((p)->weight)
#define DTYPE(p)     ((p)->dtype)
#define STATUS(p)    ((p)->status)

/*
 * The data structure for the ddivisor_t structure resides in extract.h
 * This structure is used in some of the speed_up code (HACK)...
 */

/*  Define the cell data structure of a double-cube divisor */
typedef struct ddivisor_cell_struct ddivisor_cell_t;
struct ddivisor_cell_struct {
    sm_row *cube1;        /* the pointer of the first cube */
    sm_row *cube2;        /* the pointer of the second cube */
    lsHandle handle1;     /* lsHandle to the list in cube1. */
    lsHandle handle2;     /* lsHandle to the list in cube2  */
    lsHandle handle;      /* lsHandle to the corresponding list */
    ddivisor_t *div;      /* the address of the corresponding divisor */
    int sis_id;           /* the id in sis */
    UNSIGNED phase;       /* the phase of this doubler-cube divisor */
    UNSIGNED baselength;  /* the baselength */
};

#define DIVISOR(p)   ((p)->div)


/*  Define the data structure of a single-cube divisor. */
typedef struct single_cube_divisor_struct sdivisor_t;
struct single_cube_divisor_struct {
    SHORT col1;             /* the 1st column number of a single-cube divisor */
    SHORT col2;             /* the 2nd column number of a single-cube divisor */
    SHORT coin;             /* the coincidence of a single-cube divisor */
};

#define COIN(p)      ((p)->coin)


/* Define the data structure of double-cube divisor set. */
typedef struct ddivisor_set_struct ddset_t;
struct ddivisor_set_struct {
    lsList DDset;          /* storing all double-cube divisors. */
    sm_row *node_cube_set; /* storing the cubes for each node in sis */
    sm_matrix *D112_set;   /* storing D112 type divisors */
    sm_matrix *D222_set;   /* storing D222 type divisors */
    sm_matrix *D223_set;   /* storing D223 type divisors */
    sm_matrix *D224_set;   /* storing D224 type divisors */
    sm_matrix *Dother_set; /* storing other types divisors */
};

/* Define the data structure of single-cube divisor set. */
typedef struct sdivisor_set_struct sdset_t;
struct sdivisor_set_struct {
    heap_t *heap;     /* storing all single-cube divisor */
    int index;        /* the largest column number considered so far */
    lsList columns;   /* the cols unconsidered */
    lsList col_set;   /* the cols considered */
};

/* Define the data structure of col_cell */
typedef struct col_cell_struct col_cell_t;
struct col_cell_struct {
    SHORT num;        /* column number */
    int length;       /* column length */
    lsHandle handle;  /* lsHandle */
};

/* Define the data structure for each cube in sparse matrix. */
typedef struct sm_cube_cell_struct sm_cube_t;
struct sm_cube_cell_struct {
    lsList div_set;      /* storing the dd_cells affected by this cube */
    lsHandle cubehandle; /* pointing to the corresponding node_cube_set */
    int sis_id;          /* the id in sis  */
};

extern int ONE_PASS;
extern int FX_DELETE;
extern int LENGTH1;
extern int LENGTH2;
extern int DONT_USE_WEIGHT_ZERO;

extern int twocube_timeout_occured;

int extract_ddivisor( sm_matrix *B,
								 int index,
								 int row,
								 ddset_t *ddivisor_set );
sm_cube_t *sm_cube_alloc();

ddset_t* ddset_alloc_init();
void ddset_free( ddset_t *ddivisor_set );
void sm_cube_free( register sm_cube_t *sm_cube );

int fast_extract( sm_matrix *B,
							 ddset_t *ddivisor_set );

array_t* find_sdivisor_fanout( sm_matrix *B,
										  rect_t *rect );
#if 0
void sd_reexpress( sm_matrix *B,
							   rect_t *rect,
							   int dd112_on,
							   int index,
							   int *newcolumn );
array_t* dd_reexpress( sm_matrix* B,
								  ddivisor_t* divisor,
								  int* node_index );
#endif
array_t* find_ddivisor_fanout( ddivisor_t *divisor );
ddivisor_t *ddivisor_alloc();
void ddivisor_free(register ddivisor_t *ddivisor);
void ddivisor_cell_free(ddivisor_cell_t *ddivisor_cell);
ddivisor_cell_t *ddivisor_cell_alloc();
ddivisor_t *gen_ddivisor(sm_row *cube1, sm_row *cube2, ddivisor_cell_t *dd_cell);
ddivisor_t *check_append(register ddivisor_t *div,
			 ddset_t *ddivisor_set,
			 ddivisor_cell_t *dd_cell);
int decide_dtype(register ddivisor_t *div);
lsList choose_check_set(register ddivisor_t *divisor, ddset_t *ddivisor_set);
int check_exist(ddivisor_t *div1,ddivisor_t *div2,int *phase);
void clear_row_element(register sm_row *c1, register sm_row *c2);
void hash_ddset_table(ddivisor_t *divisor, lsList check_set);
sdset_t *sdset_alloc();
void sdset_free(sdset_t *sdset);
sdivisor_t *extract_sdivisor(sm_matrix *B, sdset_t *sdset, int wdmax);

}


#endif //_SIS_FAST_EXTRACT_INT_H_

