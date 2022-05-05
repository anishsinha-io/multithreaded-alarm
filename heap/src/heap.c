#include "heap.h"

enum heap_stat heap(binary_heap_t *heap, size_t data_size, cmpfunc comparator, printfunc printer) {
    if (!heap) return H_NULLPTR_ERR;
    heap->capacity   = 10;
    heap->size       = 0;
    heap->data       = malloc(sizeof(void *) * heap->capacity);
    heap->comparator = comparator;
    heap->printer    = printer;
    heap->data_size  = data_size;
    heap->max        = NULL;
    pthread_mutex_init(&heap->mutex, NULL);
    pthread_cond_init(&heap->cond, NULL);
    return H_OK;
}

static int new_capacity(int capacity) {
    if (capacity < 10) return capacity * 2;
    if (capacity < 100) return (int) (capacity * 1.5);
    if (capacity < 1000) return (int) (capacity * 1.1);
    return (int) (capacity * 1.01);
}

static enum heap_stat heap_resize(binary_heap_t *heap, int new_capacity) {
    if (!heap) return H_NULLPTR_ERR;
    if (new_capacity < 0) return H_INVALID_CAPACITY;
    heap->data = realloc(heap->data, sizeof(void *) * new_capacity);
    return H_OK;
}

enum heap_stat heap_insert(binary_heap_t *heap, void *key) {
    if (!heap) return H_NULLPTR_ERR;
    if (heap->capacity == heap->size)
        if ((heap_resize(heap, new_capacity(heap->capacity))) != H_OK)
            return H_INSERT_ERR;
    heap->data[heap->size++] = key;
    int new_key_index        = heap->size - 1;
    int swap_index           = (heap->size - 1) / 2;
    while (heap->comparator(heap->data[new_key_index], heap->data[swap_index]) == true && new_key_index != swap_index) {
        void *tmp = heap->data[new_key_index];
        heap->data[new_key_index] = heap->data[swap_index];
        heap->data[swap_index]    = tmp;
        new_key_index = swap_index;
        swap_index    = (int) (floor((double) new_key_index / 2));
    }

    /*
     * wake the alarm thread if it is not busy (that is, if current alarm is 0, signifying that it's waiting for work),
     * or if the new alarm comes before the one on which the alarm thread is waiting
     */
    if (!heap->max) heap->max = key;
    if (heap->size > 1 && heap->comparator(key, heap->max)) {
        heap->max = key;
        int status = pthread_cond_signal(&heap->cond);
        if (status != 0) err_abort(status, "signal cond");
    }
    return H_INSERT_OK;
}

enum heap_stat heap_delete(binary_heap_t *heap, void *buf) {
    if (!heap || !buf) return H_NULLPTR_ERR;
    if (heap->size == 0) return H_UNDERFLOW_ERR;
    memcpy(buf, heap->data[0], heap->data_size);
    heap->data[0]              = heap->data[heap->size - 1];
    heap->data[heap->size - 1] = NULL;
    heap->size--;
    if (heap->size < heap->capacity / 2) if (heap_resize(heap, heap->capacity / 2) != H_OK) return H_DELETE_ERR;
    int root_index             = 0, itr = 1, swap_index = (int) pow(2, itr) - 1;
    while (swap_index < heap->size && heap->comparator(heap->data[root_index], heap->data[swap_index]) == false) {
        void *tmp = heap->data[root_index];
        heap->data[root_index] = heap->data[swap_index];
        heap->data[swap_index] = tmp;
        root_index = swap_index;
        itr++;
        swap_index = (int) pow(2, itr) - 1;
    }
    if (heap->size == 0) heap->max = NULL;
    else heap->max = heap->data[0];
    return H_DELETE_OK;
}

void print_heap(binary_heap_t *heap) {
    for (int i = 0; i < heap->size; i++) heap->printer(heap->data[i]);
}