/* LINTLIBRARY */

/* /fats/tools/hsv/dd/st.h,v 1.1.1.1 1995/06/06 05:43:04 fabio Exp */

#ifndef ST_INCLUDED
#define ST_INCLUDED

typedef struct st_table_entry {
  char *key;
  char *record;
  struct st_table_entry *next;
} st_table_entry;

typedef struct st_table {
  int (*compare)();
  int (*hash)();
  int num_bins;
  int num_entries;
  int max_density;
  int reorder_flag;
  double grow_factor;
  st_table_entry **bins;
} st_table;

typedef struct st_generator {
  st_table *table;
  st_table_entry *entry;
  int index;
} st_generator;

#define st_is_member(table,key) st_lookup(table,key,(char **) 0)
#define st_count(table) ((table)->num_entries)

enum st_retval {ST_CONTINUE, ST_STOP, ST_DELETE};

int st_delete(), st_insert(), st_foreach(), st_gen();
int st_lookup(), st_find_or_add(), st_find(), st_add_direct();
int st_strhash(), st_numhash(), st_ptrhash(), st_numcmp(), st_ptrcmp();
st_table *st_init_table(), *st_init_table_with_params(), *st_copy();
st_generator *st_init_gen();
void st_free_table(), st_free_gen();


#define ST_DEFAULT_MAX_DENSITY 5
#define ST_DEFAULT_INIT_TABLE_SIZE 11
#define ST_DEFAULT_GROW_FACTOR 2.0
#define ST_DEFAULT_REORDER_FLAG 0

#define st_foreach_item(table, gen, key, value) \
  for(gen=st_init_gen(table); st_gen(gen,key,value) || (st_free_gen(gen),0);)

/* hc : all int st routines return ST_OUT_OF_MEM if memory allocation fails */
/*		routines return pointers return NIL pointer for out-of-memory. */
#define	ST_OUT_OF_MEM	-10000

#endif /* ST_INCLUDED */
