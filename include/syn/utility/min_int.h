/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */

#ifndef _SIS_MIN_INT_H_
#define _SIS_MIN_INT_H_

#include "ros.h"

namespace SYN_SIS
{

/* Functions defined in  minimize.c */

/* minimize.c */ extern pcover nocomp(pcover F, pcover D1, int obj);
/* minimize.c */ extern pcover nocomp_bound();
/* minimize.c */ extern pcover snocomp(pcover F, pcover D1);
/* minimize.c */ extern pset_family ncp_unate_compl();
/* minimize.c */ extern pset_family ncp_unate_complement(pset_family A, int max_lit);
/* minimize.c */ extern void ncp_compl_special_cases();
/* minimize.c */ extern pcover nc_super_gasp(pcover F, pcover D, cost_t *cost);
/* minimize.c */ extern pcover nc_all_primes(pcover F);
/* minimize.c */ extern pcover nc_make_sparse(pcover F, pcover D);
/* minimize.c */ extern pcover nc_expand_gasp();
/* minimize.c */ extern void nc_expand1_gasp(pcover F, pcover D, pcover Foriginal, int c1index, pcover *G);
/* minimize.c */ extern pcover nc_last_gasp(pcover F, pcover D, cost_t *cost);
/* minimize.c */ extern pcover nc_reduce_gasp();
/* minimize.c */ extern pcover nc_reduce_gasp(pcover F, pcover D);
/* minimize.c */ extern pcover nc_random_order();
/* minimize.c */ extern pcover nc_permute();
/* minimize.c */ extern pcube nc_reduce_cube(IN pcube *FD, IN pcube p);
/* minimize.c */ extern pcube nc_sccc(INOUT pcube *T, IN int level);
/* minimize.c */ extern esp_bool nc_sccc_special_cases(INOUT pcube *T, OUT pcube *result, IN int level);
/* minimize.c */ extern pcover ncp_expand(INOUT pcover F, IN pcover Fold, IN esp_bool nonsparse);
/* minimize.c */ extern void ncp_expand1(pcover BB, pcover CC, pcube RAISE, pcube FREESET, 
			pcube OVEREXPANDED_CUBE, pcube SUPER_CUBE,
		pcube INIT_LOWER, int *num_covered, pcube c);
/* minimize.c */ extern pcover nc_first_expand(INOUT pcover F, IN esp_bool nonsparse);
/* minimize.c */ extern void rem_unnec_r_cubes(pcube *T);
/* dcsimp.c */ extern pcover dcsimp(pcover F,pcover Old_D);

}


#endif //_SIS_MIN_INT_H_
