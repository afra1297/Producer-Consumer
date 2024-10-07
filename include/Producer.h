#ifndef PRODUCER_H
#define PRODUCER_H

#include "BoundedBuffer.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_PRODUCER_STRINGLEN 100

// Struct for producer input
typedef struct {
    int id;
    int numStrings;
    BoundedBuffer *buffer;
} ProducerInput;

// Function to create strings and input them into the Bounded Buffer
void * produceStrings(void *input);

#endif // PRODUCER_H

