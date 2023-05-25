#ifndef MEMORY_H 
#define MEMORY_H


#include "common.h"


#define ArrayInitCapacity 8

typedef struct ArrayList {
    void *data;
    U64 counter;
    U64 capacity;
} ArrayList;


#define LIST_ADD(class) \
    struct InternalList_ ##class {  \
        class *data;                \
        U64 counter;                \
        U64 capacity;               \
    }

#define List(class) struct InternalList_ ##class


#define NEW_LIST(type) (List(type) *)new_array_list(type)

void *_arr_push(ArrayList *arr, U64 n);
#define arr_push(arr, type) (type *)_arr_push((arr), sizeof(type))

#define LIST_PUSH(list, item) _arr_push( (ArrayList *)(list), sizeof((item))); \
    (list)->data[(list)->counter - 1] = item

#define LIST_GET(list, n) (list)->data[(n)]
#define LIST_GET_PTR(list, n) &(list)->data[(n)]


void arr_free(ArrayList *arr);
#define LIST_FREE(list) arr_free( (ArrayList *)list)

#define FOR_ALL(item, list)                 \
    for(int i__=(item=(list)->data[0],0);   \
    i__<(list)->counter;                    \
    item=(list)->data[++i__])


ArrayList *_new_array_list(U64 n);
#define new_array_list(type) _new_array_list(sizeof(type))


#define arr_get(arr, type, n) ((type *)(arr)->data)[(n)]
#define arr_get_ptr(arr, type, n) (((type *)(arr)->data) + (n))


void *_arr_pop(ArrayList *arr, U64 n);
#define arr_pop(arr, type) (type *)_arr_pop((arr), sizeof(type))


unsigned long hash_function(void *key, int len);
unsigned long hash_string(char *key);


#endif  // MEMORY_H
