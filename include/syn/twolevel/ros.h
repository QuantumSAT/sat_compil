/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */

#ifndef _SIS_ROS_H_
#define _SIS_ROS_H_

#include "espresso.h"

namespace SYN_SIS
{

#define LEFT 0
#define RIGHT 1
#define NONUNATE 0
#define UNATE 1
#define cdist0bv(p, q, var) \
((GETINPUT(p, var)) & (GETINPUT(q, var)))
#define ROS_ZEROS 20
#define ROS_MEM_SIZE 1000000

/* Node to store intermediate nodes of unate tree */
typedef struct nc_node_struct {
   struct nc_node_struct *child[2];
   pset part_cube[2];
   pset *cubelist;
   pset cof;
   int var;
} nc_node_t, *pnc_node; 

/* ros_holder to hold ROS */
typedef struct ros_holder_struct {
   pset_family ros;
   pcube ros_cube;
   int count;
} ros_holder_t, *pros_holder;


/* Global variables used by routines in ros */
extern pcube *nc_tmp_cube;	/* Temporary cubes to be used in this file */

extern int N_level;            /* Some processing to reduce the size of
				  cofactors is done every N_level levels */
extern pnc_node root;		/* Root node of the unate tree */

extern pcover ROS;		/* Reduced off set */

/*extern int ROS_max_size_store;*/	/* Upped limit on the size of ROS that can
                                   be stored for future use */

/* Functions used in ros */

/* ros.c */ extern void get_ROS1(pcube p, pcube overexpanded_cube, pnc_node pNode);
/* ros.c */ extern pcover get_ROS(int i, pcover F, pcover Fold);
/* ros.c */ extern void init_ROS(pcover F, pcover D, int maxLevel, int nLevel, int rosZeros);
/* ros.c */ extern esp_bool find_correct_ROS(pcube p);
/* ros.c */ extern void store_ROS();
/* ros.c */ extern void close_ROS();
/* ros.c */ extern void find_overexpanded_cube_dyn();
/* ros.c */ extern esp_bool find_oc_dyn_special_cases(pcube p, pcube overexpanded_cube, 
						  pcube* T, pcube dist_cube, 
						  int dist, int var, int level);
/* ros.c */ extern pcover setupBB_dyn();
/* ros.c */ extern esp_bool BB_dyn_special_cases(pcover *BB, pcube p, 
					 pcube overexpanded_cube, pcube *T, int level);
/* ros.c */ extern void find_overexpanded_cube();
/* ros.c */ extern esp_bool find_oc_special_cases(pcube p, pcube overexpanded_cube, pnc_node pNode, 
					  pcube dist_cube, int dist, int var, int level);
/* ros.c */ extern pcover setupBB(pcube p, pcube overexpanded_cube, pnc_node pNode, int level);
/* ros.c */ extern esp_bool BB_special_cases(pcover *BB, pcube p, pcube overexpanded_cube, 
				 pnc_node pNode, int level);
/* ros.c */ extern void get_OC(pcube p, pcube overexpanded_cube, pcube old_cube);
/* ros.c */ extern esp_bool partition_ROS(pcube p, pcube overexpanded_cube, 
			  int level, pcube *T, pcover *R);
/* ros.c */ extern pcover multiply2_sf(pcover A, pcover B);
/* ros.c */ extern int nc_cubelist_partition(pcube *T, pcube **A, pcube **B, 
					  unsigned int comp_debug);
/* ros.c */ extern void nc_generate_cof_tree(pnc_node pNode, pcube *T, int level);
/* ros.c */ extern pcover nc_compl_cube(register pcube p);
/* ros.c */ extern void nc_free_unate_tree(pnc_node pNode);
/* ros.c */ extern void nc_bin_cof(pcube *T, pcube **Tl, pcube **Tr, pcube cleft, pcube cright, int var);
/* ros.c */ extern void nc_cof_contain(INOUT pcube *T);
/* ros.c */ extern esp_bool cdist0v(pcube p, pcube q, int var);
/* ros.c */ extern void nc_rem_unnec_cubes(pcube p, pcube *T);
/* ros.c */ extern void nc_or_unnec(pcube c, pcube *T, pcube orred_cube);
/* ros.c */ extern esp_bool nc_is_nec(pcube c, pcube *T);
/* ros.c */ extern void nc_sf_multiply(pset_family A, pcube c, int var);
/* ros.c */ extern void nc_rem_noncov_cubes(pcube p, pcube *T);
/* ros.c */ extern void nc_compl_special_cases(pcube *T, pcover *Tbar);
/* ros.c */ extern pcube *nc_copy_cubelist(pcube *T);
/* ros.c */ extern void nc_setup_tmp_cubes(int num);
/* ros.c */ extern void nc_free_tmp_cubes(int num);

}


#endif //_SIS_ROS_H_
