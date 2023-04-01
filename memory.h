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


#define mem_push(arena_ptr, type) \
    (type *)_push_struct((arena_ptr), (sizeof(type)))

#define new_array_list(type) _new_array_list(sizeof(type))
#define arr_push(arr, type) (type *)_arr_push((arr), sizeof(type))
#define arr_pop(arr, type) (type *)_arr_pop((arr), sizeof(type))


#endif  // MEMORY_H
