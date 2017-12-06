/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */

#ifndef _SIS_ESP_H_
#define _SIS_ESP_H_

#include <stdlib.h>
#include <stdio.h>

namespace SYN_SIS
{

/*
 *   An Espresso set is a vector of bits and is implemented here as an 
 *   array of unsigned integers.  The low order bits of set[0] give the 
 *   index of the last word of set data.  The higher order bits of set[0] 
 *   are used to store data associated with the set.  The set data is
 *   contained in elements set[1] ... set[LOOP(set)] as a packed bit
 *   array.
 *
 *   A family of sets is a two-dimensional matrix of bits and is
 *   implemented with the data type "set_family".
 *
 */

/* Define the set type */
typedef unsigned int *pset;

/* Define the set family type -- an array of sets */
typedef struct set_family {
    int wsize;                  /* Size of each set in 'ints' */
    int sf_size;                /* User declared set size */
    int capacity;               /* Number of sets allocated */
    int count;                  /* The number of sets in the family */
    int active_count;           /* Number of "active" sets */
    pset data;                  /* Pointer to the set data */
    struct set_family *next;    /* For garbage collection */
} set_family_t, *pset_family;

/* pair_t describes bit-paired variables */
typedef struct pair_struct {
    int cnt;
    int *var1;
    int *var2;
} pair_t, *ppair;

/* symbolic_list_t describes a single ".symbolic" line */
typedef struct symbolic_list_struct {
    int variable;
    int pos;
    struct symbolic_list_struct *next;
} symbolic_list_t;


/* symbolic_list_t describes a single ".symbolic" line */
typedef struct symbolic_label_struct {
    char *label;
    struct symbolic_label_struct *next;
} symbolic_label_t;


/* symbolic_t describes a linked list of ".symbolic" lines */
typedef struct symbolic_struct {
    symbolic_list_t *symbolic_list;	/* linked list of items */
    int symbolic_list_length;		/* length of symbolic_list list */
    symbolic_label_t *symbolic_label;	/* linked list of new names */
    int symbolic_label_length;		/* length of symbolic_label list */
    struct symbolic_struct *next;
} symbolic_t;

/* PLA_t stores the logical representation of a PLA */
typedef struct {
    pset_family F, D, R;		/* on-set, off-set and dc-set */
    char *filename;             /* filename */
    int pla_type;               /* logical PLA format */
    pset phase;                /* phase to split into on-set and off-set */
    ppair pair;                 /* how to pair variables */
    char **label;		/* labels for the columns */
    symbolic_t *symbolic;	/* allow binary->symbolic mapping */
    symbolic_t *symbolic_output;/* allow symbolic output mapping */
} PLA_t, *pPLA;

}


#endif //_SIS_ESP_H_

