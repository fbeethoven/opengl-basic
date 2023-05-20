#ifndef MEMORY_H 
#define MEMORY_H


#include "common.h"


#define ArrayInitCapacity 8
#define MemoryCapacity 5000

typedef struct ArrayList {
    void *data;
    U64 counter;
    U64 capacity;
} ArrayList;


#define ListAdd(class) \
    struct InternalList_ ##class {  \
        class data;                 \
        U64 counter;                \
        U64 capacity;               \
    }

#define List(class) struct InternalList_ ##class

typedef struct MemoryStack MemoryStack;
struct MemoryStack {
    U64 pointer;
    MemoryStack *prev_pointer;
};


typedef struct Arena {
    void *memory;
    MemoryStack *last_pointer;
    U64 capacity;
} Arena;


#define list_new(type) \
    (type) *data = calloc(ArrayInitCapacity, sizeof(type)); \
    (List(type)) *arr = (List(type) *) malloc(sizeof(List(type))); \
    arr->data = data; \
    arr->counter = 0; \
    arr->capacity = ArrayInitCapacity; \
    arr 


Arena *init_arena(unsigned long memory_capacity);
void *_push_struct(Arena *arena, U64 n);
#define mem_push(arena_ptr, type) \
    (type *)_push_struct((arena_ptr), (sizeof(type)))

ArrayList *_new_array_list(U64 n);
#define new_array_list(type) _new_array_list(sizeof(type))

void *_arr_push(ArrayList *arr, U64 n);
#define arr_push(arr, type) (type *)_arr_push((arr), sizeof(type))

#define arr_get(arr, type, n) ((type *)(arr)->data)[(n)]
#define arr_get_ptr(arr, type, n) (((type *)(arr)->data) + (n))

void *_arr_pop(ArrayList *arr, U64 n);
#define arr_pop(arr, type) (type *)_arr_pop((arr), sizeof(type))

void arr_free(ArrayList *arr);

unsigned long hash_function(void *key, int len);
unsigned long hash_string(char *key);


#endif  // MEMORY_H
