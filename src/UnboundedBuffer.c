#include "UnboundedBuffer.h"


// Function to create a new Unbounded Buffer
UnboundedBuffer* unboundedCreate(sem_t *mutex, sem_t *full) {
    UnboundedBuffer *u_buff = (UnboundedBuffer *)malloc(sizeof(UnboundedBuffer));
    if (!u_buff) {
        return NULL;
    }
    u_buff->buffer = (char **)malloc(sizeof(char *)*2);
    if (!u_buff->buffer) {
        return NULL;
    }
    u_buff->offsetIn = 0;
    u_buff->offsetOut = 0;
    u_buff->full = full;
    u_buff->mutex = mutex;
    return u_buff;
}

// Function to insert a new string to the Unbounded Buffer
void unboundedInsert(UnboundedBuffer *u_buff, char *new_item) {
    sem_wait(u_buff->mutex);

    // Critical section
    u_buff->buffer[u_buff->offsetIn] = new_item;
    u_buff->offsetIn = u_buff->offsetIn + 1;
    u_buff->buffer = (char **)realloc(u_buff->buffer, (u_buff->offsetIn + 1)*sizeof(char *));
    if (u_buff->buffer == NULL) {
        exit(-1);
    }
    // End of critical section
    sem_post(u_buff->mutex);
    sem_post(u_buff->full);
}

// Function to remove a string from the Unbounded Buffer
char *unboundedRemove(UnboundedBuffer *u_buff) {
    sem_wait(u_buff->full);
    sem_wait(u_buff->mutex);

    // Critical section
    char *removed = u_buff->buffer[u_buff->offsetOut];
    u_buff->offsetOut = u_buff->offsetOut + 1;

    //End of critical section
    sem_post(u_buff->mutex);

    return removed;
}