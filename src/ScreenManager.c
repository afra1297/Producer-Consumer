#include "ScreenManager.h"

// Function to read from the editors queue and print to the screen
void * printScreen(void *input) {
    // Parse the input
    ScreenManagerInput *inputs = (ScreenManagerInput *)input;
    BoundedBuffer *buffer = inputs->editorsBuffer;
    int numTimes = inputs->numTimes;
    // Read and print to screen
    char *message = boundedRemove(buffer);
    int i;
    for(i = 0; i < numTimes; i++) {
        printf("%s\n", message);
        free(message);
        if (i != numTimes - 1) {
            message = boundedRemove(buffer);
        }
    }
    printf("DONE\n");
    return NULL;
}