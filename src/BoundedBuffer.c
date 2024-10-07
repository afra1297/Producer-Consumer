#include "BoundedBuffer.h"

// Function to create a new Bounded Buffer
BoundedBuffer* boundedCreate(int size, sem_t *mutex, sem_t *full, sem_t *empty) {
    // Initialize the struct
    BoundedBuffer *b_buff = (BoundedBuffer *)malloc(sizeof(BoundedBuffer));
    if (!b_buff) {
        return NULL;
    }
    b_buff->size = size;
    b_buff->buffer = (char **)malloc(size * sizeof(char *));
    if (!b_buff->buffer) {
        free(b_buff);
        return NULL;
    }
    b_buff->empty = empty;
    b_buff->full = full;
    b_buff->mutex = mutex;
    b_buff->offsetIn = 0;
    b_buff->offsetOut = 0;

    return b_buff;
}

// Function to insert a new string to the Bounded Buffer
void boundedInsert(BoundedBuffer *b_buff, char *new_item) {
    // Entry section 
    sem_wait(b_buff->empty);
    sem_wait(b_buff->mutex);

    // Critical section - adding a new item to the Bounded Buffer
    b_buff->buffer[b_buff->offsetIn] = new_item;
    b_buff->offsetIn = (b_buff->offsetIn + 1) % b_buff->size;

    // End of critical section - release semaphores and exit
    sem_post(b_buff->mutex);
    sem_post(b_buff->full);
}

// Function to remove a string from the Bounded Buffer
char *boundedRemove(BoundedBuffer *b_buff) {
    // Entry section
    sem_wait(b_buff->full);
    sem_wait(b_buff->mutex);

    // Critical section - remove item from the Bounded Buffer
    char *removed = b_buff->buffer[b_buff->offsetOut];
    b_buff->offsetOut = (b_buff->offsetOut + 1) % b_buff->size;

    // End of critical section - release semaphores and return removed item
    sem_post(b_buff->mutex);
    sem_post(b_buff->empty);
    return removed;
}