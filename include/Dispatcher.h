#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string.h>
#include "BoundedBuffer.h"
#include "Producer.h"
#include "UnboundedBuffer.h"

// Struct for dispatcher input
typedef struct {
    BoundedBuffer **producerBuffers; // Array of buffers, each buffer belongs to a different producer
    int producersNum; // Number of producers
    UnboundedBuffer **dispatcherQueues; // Array of dispatcher queues
} DispatcherInput;

// Function to sort producers messages and inserts them into dispatcher queues
void * sortMessages(void *input);

#endif // DISPATCHER_H