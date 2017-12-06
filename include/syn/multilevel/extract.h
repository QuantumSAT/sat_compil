/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SIS_EXTRACT_H_
#define _SIS_EXTRACT_H_

#include "syn/utility/sis_list.h"
//#include "syn/netlist.h"
#include "syn/twolevel/espresso.h"

namespace SYN {
  class Gate;
}

namespace SYN_SIS {

extern void ex_kernel_gen(SYN::Gate*, int (*func)(SYN::Gate*, SYN::Gate*, char*), char*);
extern void ex_subkernel_gen(SYN::Gate*, int (*func)(SYN::Gate*, SYN::Gate*, char*), int, char*);

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
  extern pset_family set_div(int nin, pset_family set_f, pset_family set_g, pset_family* set_r);
  extern pset_family set_div_single_cube(int nin, pset_family set_f, pset_family set_g, pset_family* set_r);
  extern pset_family set_div_single_literal(int nin, pset_family set_f, pset_family set_g, pset_family* set_r);
}

#endif //_SIS_EXTRACT_H_
