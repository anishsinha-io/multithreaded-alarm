#ifndef __HEAP__
#define __HEAP__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "errors.h"

// prototype to compare two heap elements
typedef bool(*cmpfunc)(const void *, const void *);

// prototype to print a heap element
typedef void(*printfunc)(const void *);

// binary heap object
typedef struct binary_heap {
    int             size, capacity;
    void            *max, **data;
    size_t          data_size;
    cmpfunc         comparator;
    printfunc       printer;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
}           binary_heap_t;

// heap status codes
enum heap_stat {
    H_OK,
    H_INSERT_OK, H_INSERT_ERR,
    H_DELETE_OK, H_DELETE_ERR,
    H_UNDERFLOW_ERR, H_OVERFLOW_ERR,
    H_INVALID_CAPACITY,
    H_NULLPTR_ERR,
};

// api
void print_heap(binary_heap_t *heap);

enum heap_stat heap(binary_heap_t *heap, size_t data_size, cmpfunc comparator, printfunc printer);
enum heap_stat heap_insert(binary_heap_t *heap, void *key);
enum heap_stat heap_delete(binary_heap_t *heap, void *buf);

#endif