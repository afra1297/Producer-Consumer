#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <string.h>
#include "BoundedBuffer.h"

// Struct for screen manager input
typedef struct {
    BoundedBuffer *editorsBuffer;
    int numTimes;
} ScreenManagerInput;

// Function to read from the editors queue and print to the screen
void * printScreen(void *input);

#endif // SCREEN_MANAGER_H