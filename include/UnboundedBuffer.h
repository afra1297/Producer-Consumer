#ifndef UNBOUNDED_BUFFER_H
#define UNBOUNDED_BUFFER_H

#include <malloc.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>

// Unbounded Buffer struct (implemented as a unbounded queue)
typedef struct {
    sem_t *mutex;
    sem_t *full;
    int offsetIn;
    int offsetOut;
    char** buffer;
} UnboundedBuffer;

// Function to create a new Unbounded Buffer
UnboundedBuffer* unboundedCreate(sem_t *mutex, sem_t *full);

// Function to insert a new string to the Unbounded Buffer
void unboundedInsert(UnboundedBuffer *u_buff, char *new_item);

// Function to remove a string from the Unbounded Buffer
char *unboundedRemove(UnboundedBuffer *u_buff);

#endif // UNBOUNDED_BUFFER_H