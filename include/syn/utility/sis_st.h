/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SIS_ST_H_
#define _SIS_ST_H_

namespace SYN_SIS
{

typedef struct st_table_entry st_table_entry;
struct st_table_entry {
    char *key;
    char *record;
    st_table_entry *next;
};

typedef struct st_table st_table;
struct st_table {
    int (*compare)(char*, char*);
    int (*hashf)(char*, int);
    int num_bins;
    int num_entries;
    int max_density;
    int reorder_flag;
    double grow_factor;
    st_table_entry **bins;
};

typedef struct st_generator st_generator;
struct st_generator {
    st_table *table;
    st_table_entry *entry;
    int index;
};

#define st_is_member(table,key) st_lookup(table,key,(char **) 0)
#define st_count(table) ((table)->num_entries)

enum st_retval {ST_CONTINUE, ST_STOP, ST_DELETE};

typedef enum st_retval (*ST_PFSR)();
typedef int (*ST_PFI)();


int st_delete(register st_table *table, register char **keyp, char **value);
int st_delete_int(register st_table *table, register int *keyp, char **value);
int st_insert(register st_table *table, register char *key, char *value);
int st_foreach(st_table *table, enum st_retval (*func)(), char *arg);
int st_gen(st_generator *gen, char **key_p, char **value_p);
int st_gen_int(st_generator *gen, char **key_p, int *value_p);
int st_lookup(st_table *table, register char *key, char **value);
int st_lookup_int(st_table *table,register char * key, int *value);
int st_find_or_add(st_table *table, char *key, char ***slot);
int st_find(st_table *table, char *key, char ***slot);
int st_add_direct(st_table *table, char *key, char *value);
int st_strhash(register char *string, int modulus);
int st_numhash(char *x, int size);
int st_ptrhash(char *x, int size);
int st_numcmp(char *x, char *y);
int st_ptrcmp(char *x, char *y);
st_table *st_init_table(int (*compare)(char*, char*), int (*hashf)(char*, int)); 
st_table *st_init_table_with_params(int (*compare)(char*, char*), 
									int (*hashf)(char*, int), int size, 
									int density, 
									double grow_factor,
									int reorder_flag);
st_table *st_copy(st_table *old_table);
st_generator *st_init_gen(st_table *table);
void st_free_table(st_table *table);
void st_free_gen(st_generator *gen);

#define ST_DEFAULT_MAX_DENSITY 5
#define ST_DEFAULT_INIT_TABLE_SIZE 11
#define ST_DEFAULT_GROW_FACTOR 2.0
#define ST_DEFAULT_REORDER_FLAG 0

#define st_foreach_item(table, gen, key, value) \
    for(gen=st_init_gen(table); st_gen(gen,key,value) || (st_free_gen(gen),0);)

#define st_foreach_item_int(table, gen, key, value) \
    for(gen=st_init_gen(table); st_gen_int(gen,key,value) || (st_free_gen(gen),0);)

}

#endif //_SIS_ST_H_


