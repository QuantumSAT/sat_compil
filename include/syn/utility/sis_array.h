/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SIS_ARRAY_H_
#define _SIS_ARRAY_H_

namespace SYN_SIS
{

typedef struct array_t {
    char *space;
    int	 num;		/* number of array elements.		*/
    int	 n_size;	/* size of 'data' array (in objects)	*/
    int	 obj_size;	/* size of each array object.		*/
    int	 index;		/* combined index and locking flag.	*/
} array_t;

array_t *array_do_alloc(int, int);
array_t *array_dup(array_t *);
array_t *array_join(array_t *, array_t *);
void array_free(array_t *);
void array_append(array_t *, array_t *);
void array_sort(array_t *, int (*)());
void array_uniq(array_t *, int (*)(), void (*)());
int array_abort(array_t *, int);
int array_resize(array_t *, int);
char *array_do_data(array_t *);

//extern unsigned int array_global_index;
extern int array_global_index;
#define array_alloc(type, number)		\
    array_do_alloc(sizeof(type), number)

#define array_insert(type, a, i, datum)		\
    (  -(a)->index != sizeof(type) ? array_abort((a),4) : 0,\
	(a)->index = (i),\
	(a)->index < 0 ? array_abort((a),0) : 0,\
	(a)->index >= (a)->n_size ? array_resize(a, (a)->index + 1) : 0,\
	*((type *) ((a)->space + (a)->index * (a)->obj_size)) = datum,\
	(a)->index >= (a)->num ? (a)->num = (a)->index + 1 : 0,\
	(a)->index = -(int)sizeof(type)	)

#define array_insert_last(type, array, datum)	\
    array_insert(type, array, (array)->num, datum)

#define array_fetch(type, a, i)			\
    (array_global_index = (i),				\
      (array_global_index >= (a)->num) ? array_abort((a),1) : 0,\
      *((type *) ((a)->space + array_global_index * (a)->obj_size)))

#define array_fetch_p(type, a, i)                       \
    (array_global_index = (i),                             \
      (array_global_index >= (a)->num) ? array_abort((a),1) : 0,\
      ((type *) ((a)->space + array_global_index * (a)->obj_size)))

#define array_fetch_last(type, array)		\
    array_fetch(type, array, ((array)->num)-1)

#define array_n(array)				\
    (array)->num

#define array_data(type, array)			\
    (type *) array_do_data(array)

}

#endif //_SIS_ARRAY_H_


