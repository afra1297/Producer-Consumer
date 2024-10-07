#ifndef CO_EDITOR_H
#define CO_EDITOR_H

#include <unistd.h>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"
#include <string.h>

// Struct for Co-Editor input
typedef struct {
    UnboundedBuffer *dispatcherQueue;
    BoundedBuffer *editorsQueue;
} CoEditorInput;

// Function that reads from the dispatcher queue, edits and writes it to the screen-manager queue
void * editMessage(void *input);

#endif // CO_EDITOR_H