/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */


#ifndef _SIS_EXTRACT_INT_H_
#define _SIS_EXTRACT_INT_H_

#include <assert.h>
#include "syn/utility/sis_array.h"
#include "syn/utility/sparse.h"
#include "syn/utility/util.h"

#include "syn/utility/sis_st.h"

namespace SYN_SIS {

typedef struct rect_struct rect_t;
struct rect_struct {
    sm_col *rows;
    sm_row *cols;
    int value;
};


/*
 *  each element of the sparse matrix contains a 'value' and identifies
 *  where it came from
 */
typedef struct value_cell_struct value_cell_t;
struct value_cell_struct {
    int value;			/* # literals in this cube */
    int sis_index;		/* which sis function */
    int cube_number;		/* which cube of that function */ 
    int ref_count;		/* number of times referenced */
};
#define VALUE(p)		((value_cell_t *) (p)->user_word)->value


/*
 *  a quick way to associate cubes (i.e., sm_row *) to small integers
 *  and back.  Used to build the kernel_cube matrix
 */
typedef struct cubeindex_struct cubeindex_t;
struct cubeindex_struct {
    st_table *cube_to_integer;
    array_t *integer_to_cube;
};

/* greedyrow.c, greedycol.c, pingpong.c */
rect_t *
greedy_row(sm_matrix *A, array_t *row_cost, array_t *col_cost, sm_row *seed);
rect_t *
greedy_col(sm_matrix *A, array_t *col_cost, array_t *row_cost, sm_col *seed);
rect_t *
ping_pong(sm_matrix *A, array_t *row_cost, array_t *col_cost);

/* kernel.c */
void kernel_extract_init();
void kernel_extract_end();
void kernel_extract_free();
int ex_is_level0_kernel(sm_matrix *A);

extern sm_matrix *kernel_cube_matrix;		/* hack */
extern array_t *co_kernel_table;		/* hack */
extern array_t *global_row_cost;		/* hack */
extern array_t *global_col_cost;		/* hack */

/* best_subk.c */
rect_t *choose_subkernel(sm_matrix *A, array_t 
				 *row_cost, 
				 array_t *col_cost, 
				 int option);

/* rect.c */
rect_t *rect_alloc();
void rect_free(rect_t *rect);
rect_t *rect_dup(rect_t *old_rect);

/* value.c */
value_cell_t *value_cell_alloc();
void value_cell_free(value_cell_t *value_cell);
void free_value_cells(sm_matrix *A);

/* sisc.c */
array_t *
find_rectangle_fanout(sm_matrix *A, rect_t *rect);
array_t *find_rectangle_fanout_cubes(sm_matrix *A, rect_t *rect, array_t **cubes);

/* cubeindex.c */
cubeindex_t *cubeindex_alloc();
void cubeindex_free(cubeindex_t *table);
int cubeindex_getindex(cubeindex_t *table, sm_row *cube);
sm_row *cubeindex_getcube(cubeindex_t *table, int index);


extern int cube_extract_debug;
extern int kernel_extract_debug;
extern int greedy_row_debug;
extern int greedy_col_debug;
extern int ping_pong_debug;

int sparse_cube_extract( sm_matrix*, int, int );
void free_value_cells( sm_matrix* );
value_cell_t* value_cell_alloc();

void kernel_extract( sm_matrix *A,
							    int sis_index,
							    int use_all_kernels );
sm_matrix *ex_rect_to_kernel( rect_t *rect );
sm_matrix *ex_rect_to_cokernel( rect_t *rect );
int sparse_kernel_extract( int value_threshold,
									  int use_best_subkernel );
int overlapping_kernel_extract( int value_threshold,
										   int use_best_subkernel );

int ex_divide_function_into_network( sm_matrix* func,
												array_t* fanout,
												array_t* cubes,
												int debug );
const char *ex_map_index_to_name( int i );

void gen_all_rectangles( sm_matrix* M,
								    int (*record)( sm_matrix *co_rect,
												   register rect_t *rect,
												   char *state_p ),
									char *state );

#define sisout stdout

}


#endif //_SIS_EXTRACT_INT_H_

