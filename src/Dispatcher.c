#include "Dispatcher.h"

// Function to find the type of a string that was given by the producer
// Thirs token of the string is its type: SPORTS, NEWS or WEATHER
char * findType(char *producerString) {
    char *token;
    // Get the third token using space (" ") as the delimiter
    token = strtok(producerString, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");

    return token;
}

// Function to sort producers messages and inserts them into dispatcher queues
void * sortMessages(void *input) {
    // Parse inputs
    DispatcherInput *inputs = (DispatcherInput *)input;
    BoundedBuffer **buffersArr = inputs->producerBuffers;
    UnboundedBuffer **dispatcherQueues = inputs->dispatcherQueues;
    int producersNum = inputs->producersNum;

    // Set variables for round robin algorithm on the producer buffers 
    int doneCounter = 0;
    char doneProducer[doneCounter];
    int i;
    for (i = 0; i < producersNum; i++) {
        doneProducer[i] = 0;
    }
    i = 0;
    // Round robin algorithm on the producers buffers
    while (1) {
        // Check if finished cycle of producers
        if (i == producersNum) {
            // If all producers are done - exit
            if (doneCounter == producersNum) {
                break;
            } else {
                // If some producers did not finish - start from the beginning
                i = 0;
                doneCounter = 0;
            }
        }
        char *message;
        // If this producer is not done
        if (!doneProducer[i]) {
            message =  boundedRemove(buffersArr[i]);
            if (!strcmp(message, "DONE")) {
                doneProducer[i] = 1;
            } else {
                // Create copy of article
                char *copy = malloc((sizeof(char) + 1)*strlen(message));
                if (copy == NULL) {
                    exit(-1);
                }
                strcpy(copy, message);
                char *type = findType(copy);
                if (!strcmp(type, "SPORTS")) {
                    unboundedInsert(dispatcherQueues[0], message);
                } else if (!strcmp(type, "NEWS")) {
                    unboundedInsert(dispatcherQueues[1], message);
                } else {
                    unboundedInsert(dispatcherQueues[2], message);
                }
                free(copy);
            }
        } else {
            doneCounter++;
        }
        i++;
    }
    // If all the producer buffers are done
    unboundedInsert(dispatcherQueues[0], "DONE");
    unboundedInsert(dispatcherQueues[1], "DONE");
    unboundedInsert(dispatcherQueues[2], "DONE");

    return NULL;
}