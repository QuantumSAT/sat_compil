/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */

#ifndef _SIS_MINCOV_H_
#define _SIS_MINCOV_H_

namespace SYN_SIS
{

/* exported */
extern sm_row *sm_minimum_cover(sm_matrix *A, int *weight, int heuristic, int debug_level);

}


#endif //_SIS_MINCOV_H_
