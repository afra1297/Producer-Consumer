#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <semaphore.h>
#include <malloc.h>

// Bounded Buffer struct (implemented as a bounded queue)
typedef struct {
    char **buffer;
    sem_t *mutex;
    sem_t *full;
    sem_t *empty;
    int size;
    int offsetIn;
    int offsetOut;
   
} BoundedBuffer;

// Function to create a new Bounded Buffer
BoundedBuffer* boundedCreate(int size, sem_t *mutex, sem_t *full, sem_t *empty);

// Function to insert a new string to the Bounded Buffer
void boundedInsert(BoundedBuffer *b_buff, char *new_item);

// Function to remove a string from the Bounded Buffer
char *boundedRemove(BoundedBuffer *b_buff);

#endif // BOUNDED_BUFFER_H