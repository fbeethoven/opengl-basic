#include "memory.h"


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
    void * item = (U8 *)arr->data + (n * arr->counter);
    memset(item, 0, n);
    arr->counter++;
    return item;
}


void *_arr_pop(ArrayList *arr, U64 n) {
    if (arr->counter == 0 ) {
        printf("[ERROR] Array is empty\n");
        return 0;
    }
    void *item = (U8 *)arr->data + (arr->counter * n);
    arr->counter--;
    return item;
}

void arr_free(ArrayList *arr) {
    free(arr->data);
    free(arr);
}


// FNV-1a hash function in 64 bits
unsigned long hash_function(void *key, int len) {
    unsigned long hash = 0xcbf29ce484222325;
    // unsigned long fnv_prime = 0x00000100000001B3;

    for(unsigned int i=0; i<len; i++) {
        hash ^= *(unsigned char *)key + i;
        hash *= 0x00000100000001B3;
    }
    return hash;
}


// DJB2
unsigned long hash_string(char *str) {
    unsigned long hash = 5381;
    // int c = *str++;

    for(
        int c=*(unsigned char *)str++;
        c;
        c = *(unsigned char *)str++
    ) {
        // hash = ((hash << 5) + hash) + c;
        hash = (hash * 33) ^ c;
    }

    return hash;
}

