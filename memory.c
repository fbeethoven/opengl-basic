#include "memory.h"


Arena *init_arena() {
    void *memory = calloc(1, MemoryCapacity);
    MemoryStack *last_pointer = calloc(1, sizeof(MemoryStack));
    Arena *arena = calloc(1, sizeof(Arena));
    arena->memory = memory;
    arena->last_pointer = last_pointer;
    arena->capacity = MemoryCapacity;
    return arena;
}


void *_push_struct(Arena *arena, U64 n) {
    U32 pointer = arena->last_pointer->pointer;
    if (pointer + n >= arena->capacity) {
        printf(
            "[ERROR] Could not allocate %lu memory, only %lu space left\n",
            n, arena->capacity - pointer
        );
        return (void *)0;
    }
    arena->last_pointer->pointer += n;
    return arena->memory + pointer;
}


#define mem_push(arena_ptr, type) \
    (type *)_push_struct((arena_ptr), (sizeof(type)))


ArrayList *_new_array_list(U64 n) {
    void *data = calloc(ArrayInitCapacity, n);
    ArrayList *arr = malloc(sizeof(ArrayList));
    arr->data = data;
    arr->counter = 0;
    arr->capacity = ArrayInitCapacity;
    return arr;
}



void *_arr_push(ArrayList *arr, U64 n) {
    if ( arr->counter >= arr->capacity) {
        arr->data = realloc(arr->data, 2 * arr->capacity * n); 
        arr->capacity *= 2;
    }
    void * item = arr->data + (n * arr->counter);
    memset(item, 0, n);
    arr->counter++;
    return item;
}


void *_arr_pop(ArrayList *arr, U64 n) {
    if (arr->counter == 0 ) {
        printf("[ERROR] Array is empty\n");
        return 0;
    }
    void *item = arr->data + (arr->counter * n);
    arr->counter--;
    return item;
}

