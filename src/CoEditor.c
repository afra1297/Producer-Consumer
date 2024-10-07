#include "CoEditor.h"

// Function that reads from the dispatcher queue, edits and writes it to the screen-manager queue
void * editMessage(void *input) {
    // Parse inputs
    CoEditorInput *inputs = (CoEditorInput *)input;
    UnboundedBuffer *dispatcherQueue = inputs->dispatcherQueue;
    BoundedBuffer *editorsQueue = inputs->editorsQueue;
    char *message = "";
    while (strcmp(message, "DONE")) {
        if (strcmp(message, "")) {
            // Edit - simulated by blocking for one-tenth (0.1) of a second
            sleep(0.1);
            // Write the message to the Screen-Manager queue
            boundedInsert(editorsQueue, message);
        }
        // Read message from the Dispatcher queue
        message = unboundedRemove(dispatcherQueue);
    }
    return NULL;
}
