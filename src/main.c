#include <pthread.h>
#include <stdlib.h>
#include "BoundedBuffer.h"
#include "UnboundedBuffer.h"
#include "Producer.h"
#include "Dispatcher.h"
#include "CoEditor.h"
#include "ScreenManager.h"
#include <ctype.h>

#define NUM_CO_EDITORS 3
#define CONFIG_FILE_MAX_LEN 100

// Global thread variables

// Producers
pthread_t *producersArr; // Array of Producer threads
pthread_attr_t *producerAttrs;
// Dispatcher
pthread_t dispatcher;
pthread_attr_t dispatcherAttr;
// Co-Editors
pthread_t coEditors[NUM_CO_EDITORS]; // Array of Co-Editors
pthread_attr_t editorsAttrs[NUM_CO_EDITORS];
// Screen Manager
pthread_t screenManager;
pthread_attr_t managerAttr;

// Global semaphore variables

// Buffers semaphores
sem_t *mutexArray;
sem_t *fullArray;
sem_t *emptyArray;
// Co-Editor semaphores
sem_t *editorMutexArray;
sem_t *editorFullArray;
// Screen Manager semaphores
sem_t screenMutex;
sem_t screenFull;
sem_t screenEmpty;

// Function to find the number of producers in the file
int findNumProducers(char *path) {
    FILE *configFile = fopen(path, "r");
    if (configFile == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Set variables for iterating over the file
    char line[CONFIG_FILE_MAX_LEN];
    int producersNum = 0;

    // Read lines from the file and count the number of times "Producer" appears
    while (fgets(line, sizeof(line), configFile) != NULL) {
        // Convert line to lowercase for case-insensitive comparison
        char *ptr = line;
        while (*ptr) {
            *ptr = tolower((unsigned char)*ptr);
            ptr++;
        }
        if (strstr(line, "producer") != NULL) {
            producersNum++;
        }
    }
    fclose(configFile);
    return producersNum;
}

// Function to initialize the producer queue sizes and their number of strings 
void initializeQueues(char *path, int *producerIds, int *producerQueueSizes, int *producerNumStrings, int *editorQueueSize, int numProds) {
    FILE *configFile = fopen(path, "r");
    if (configFile == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Set variables for iterating over the file
    char line[CONFIG_FILE_MAX_LEN];
    int producer = 0;
    int state = 0; // To keep track of which number we're reading for each producer

    // Go over each line of the file
    while (fgets(line, sizeof(line), configFile)) {
        // Skip empty lines
        if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
            continue;
        }

        // Extract the number from the line
        int numberInLine = -1;
        char *ptr = line;
        while (*ptr) {
            if (isdigit(*ptr)) {
                numberInLine = strtol(ptr, &ptr, 10);
                break;
            }
            ptr++;
        }
        // If there are no number in the line
        if (numberInLine == -1) {
            continue;
        }
        // If producer counter didn't reach total number of producers - extract the information(numbers)
        if (producer < numProds) {
            switch (state) {
                case 0: // Producer Id
                    producerIds[producer] = numberInLine;
                    break;
                case 1: // Number of strings the producer makes
                    producerNumStrings[producer] = numberInLine;
                    break;
                case 2: // Size of producer queue
                    producerQueueSizes[producer] = numberInLine;
                    producer++; // Move to the next producer after reading all three numbers
                    break;
            }
            state = (state + 1) % 3; // Increment state and reset after 3
        } else { // Read all producers - the last line indicates Co-Editor queue size
            *editorQueueSize = numberInLine;
        }
    }

    fclose(configFile);
}

// Function to initialize all Producers - allocates memory for threads and semaphores and initializes them
void initializeProducers(int numProds, int *producerQueueSizes) {
    // Allocate memory for threads
    producersArr = (pthread_t *)malloc(numProds * sizeof(pthread_t));
    producerAttrs = (pthread_attr_t *)malloc(numProds * sizeof(pthread_attr_t));
    // Allocate memory for semaphores
    mutexArray = (sem_t *)malloc(numProds * sizeof(sem_t));
    fullArray = (sem_t *)malloc(numProds * sizeof(sem_t));
    emptyArray = (sem_t *)malloc(numProds * sizeof(sem_t));
    // Initialize threads and semaphores
    for (int i = 0; i < numProds; i++) {
        pthread_attr_init(&producerAttrs[i]);
        sem_init(&mutexArray[i], 0, 1);
        sem_init(&fullArray[i], 0, 0);
        sem_init(&emptyArray[i], 0, producerQueueSizes[i]);
    }
}

// Function to initialize Dispatcher 
void initializeDispatcher() {
    pthread_attr_init(&dispatcherAttr);
}

// Function to initialize Co-Editors - allocates memory for semaphores and initializes them
void initializeCoEditors() {
    // Allocate memory for semaphores
    editorMutexArray = (sem_t *)malloc(NUM_CO_EDITORS * sizeof(sem_t));
    editorFullArray = (sem_t *)malloc(NUM_CO_EDITORS * sizeof(sem_t));
    // Initialize threads and semaphores
    for (int i = 0; i < NUM_CO_EDITORS; i++) {
        pthread_attr_init(&editorsAttrs[i]);
        sem_init(&editorMutexArray[i], 0, 1);
        sem_init(&editorFullArray[i], 0, 0);
    }
}

// Function to initialize Screen Manager - initializes threads and semaphores
void initializeScreenManager(int editorQueueSize) {
    // Initialize Screen Manager thread and semaphores
    pthread_attr_init(&managerAttr);
    sem_init(&screenMutex, 0, 1);
    sem_init(&screenFull, 0, 0);
    sem_init(&screenEmpty, 0, editorQueueSize);
}

// Function responsible for creating and initializing Producer threads
void createProducers(int numProds, ProducerInput **producerInput, BoundedBuffer **producerQueueArr, int *producerIds, int *producerQueueSizes,
 int *producerNumStrings) {
    for (int i = 0; i < numProds; i++) {
        producerQueueArr[i] = boundedCreate(producerQueueSizes[i], &mutexArray[i], &fullArray[i], &emptyArray[i]);
        // Allocate memory for ProducerInput
        producerInput[i] = (ProducerInput *)malloc(sizeof(ProducerInput));
        if (producerInput[i] == NULL) {
            perror("Failed to allocate memory for ProducerInput");
            exit(EXIT_FAILURE);
        }
        // Initialize ProducerInput
        producerInput[i]->numStrings = producerNumStrings[i];
        producerInput[i]->id = producerIds[i];
        producerInput[i]->buffer = producerQueueArr[i];
        // Create Producer thread
        pthread_create(&producersArr[i], &producerAttrs[i], produceStrings, (void *)producerInput[i]);
    }
}

// Function initializes the Dispatcher queues and starts the dispatcher thread
void createDispatcher(DispatcherInput *dispInput, BoundedBuffer **producerQueueArr, UnboundedBuffer **dispatcherQueueArr, int numProds) {
    for (int i = 0; i < NUM_CO_EDITORS; i++) {
        // Create unbounded buffer for each Co-Editor
        dispatcherQueueArr[i] = unboundedCreate(&editorMutexArray[i], &editorFullArray[i]);
    }
    // Initialize DispatcherInput fields
    dispInput->producersNum = numProds;
    dispInput->producerBuffers = producerQueueArr;
    dispInput->dispatcherQueues = dispatcherQueueArr;
    // Create Dispatcher thread
    pthread_create(&dispatcher, &dispatcherAttr, sortMessages, (void *)dispInput);
}

// Function responsible for creating and initializing co-editor threads
void createCoEditors(CoEditorInput **coEditorInputs, UnboundedBuffer **dispatcherQueueArr, BoundedBuffer *editorsQueue) {
    for (int i = 0; i < NUM_CO_EDITORS; i++) {
        // Allocate memory for CoEditorInput:
        coEditorInputs[i] = (CoEditorInput *)malloc(sizeof(CoEditorInput));
        if (coEditorInputs[i] == NULL) {
            perror("Failed to allocate memory for CoEditorInput");
            exit(EXIT_FAILURE);
        }
        // Initialize CoEditorInput fields
        coEditorInputs[i]->dispatcherQueue = dispatcherQueueArr[i];
        coEditorInputs[i]->editorsQueue = editorsQueue;
        // Create Co-Editor thread
        pthread_create(&coEditors[i], &editorsAttrs[i], editMessage, (void *)coEditorInputs[i]);
    }
}

// Function responsible for setting up and initializing the screen manager thread
void createScreenManager(ScreenManagerInput *managerInputs, BoundedBuffer *editorsQueue, int totalStrings) {
    // Initialize ScreenManagerInput fields
    managerInputs->editorsBuffer = editorsQueue;
    managerInputs->numTimes = totalStrings;
    // Create Screen Manager thread
    pthread_create(&screenManager, &managerAttr, printScreen, (void *)managerInputs);
}

// Function responsible for ensuring that the main thread waits for all other threads to complete before continuing
void joinThreads(int numProds) {
    // Joining Producer threads
    for (int i = 0; i < numProds; i++) {
        pthread_join(producersArr[i], NULL);
    }
    // Joining Dispatcher thread
    pthread_join(dispatcher, NULL);
     // Joining Co-Editor threads
    for (int i = 0; i < NUM_CO_EDITORS; i++) {
        pthread_join(coEditors[i], NULL);
    }
     // Joining Screen Manager thread
    pthread_join(screenManager, NULL);
}

// Function to clean up resources allocated for producers 
void cleanUpProducers(int numProds, BoundedBuffer **producerQueueArr, ProducerInput **producerInput) {
    for (int i = 0; i < numProds; i++) {
        sem_destroy(&mutexArray[i]);
        sem_destroy(&fullArray[i]);
        sem_destroy(&emptyArray[i]);
        free(producerQueueArr[i]->buffer);
        free(producerQueueArr[i]);
        free(producerInput[i]);
    }
    free(mutexArray);
    free(fullArray);
    free(emptyArray);
}

// Function to clean up resources allocated for the dispatcher and its queues
void cleanUpDispatcher(UnboundedBuffer **dispatcherQueueArr) {
    for (int i = 0; i < NUM_CO_EDITORS; i++) {
        sem_destroy(&editorMutexArray[i]);
        sem_destroy(&editorFullArray[i]);
        free(dispatcherQueueArr[i]->buffer);
        free(dispatcherQueueArr[i]);
    }
    free(editorMutexArray);
    free(editorFullArray);
}

// Function to clean up resources allocated for the screen manager
void cleanUpScreenManager(BoundedBuffer *editorsQueue, ScreenManagerInput *managerInputs) {
    free(editorsQueue->buffer);
    free(editorsQueue);
    free(managerInputs);
}


int main(int argc, char* argv[]) {
    // Check right argument (single configuration file) provided
    if (argc != 2) {
        fprintf(stderr, "Error: Please provide a single configuration file\n");
        exit(EXIT_FAILURE);
    }
    // Initialize number of Producers and Queues
    int numProds = findNumProducers(argv[1]); // Number of producers
    int producerIds[numProds]; // Array of producers Id
    int producerQueueSizes[numProds];  // Array of producers queue sizes
    int producerNumStrings[numProds];  // Array of number of strings each producer make
    int editorQueueSize;
    initializeQueues(argv[1], producerIds ,producerQueueSizes, producerNumStrings, &editorQueueSize, numProds);
    int totalStrings = 0;
    // Count number of total strings produced by all producers
    for (int i = 0; i < numProds; i++) {
        totalStrings += producerNumStrings[i];
    }
    // Initizlize Bounded and Unbounded Queues
    BoundedBuffer *producerQueueArr[numProds]; // Array of bounded producer queues
    UnboundedBuffer *dispatcherQueueArr[NUM_CO_EDITORS]; // Array of unbounded dispatcher queues (3 queues - S/N/W)
    BoundedBuffer *editorsQueue; // Co-Editors queue, used by Screen Manager
    
    // Initialize input structs
    ProducerInput *producerInput[numProds];
    DispatcherInput *dispInput = (DispatcherInput *)malloc(sizeof(DispatcherInput));
    CoEditorInput *coEditorInputs[NUM_CO_EDITORS];
    ScreenManagerInput *managerInputs = (ScreenManagerInput *)malloc(sizeof(ScreenManagerInput));
    
    // Initialize all components
    initializeProducers(numProds, producerQueueSizes);
    initializeDispatcher();
    initializeCoEditors();
    initializeScreenManager(editorQueueSize);

    // Create all components
    editorsQueue = boundedCreate(editorQueueSize, &screenMutex, &screenFull, &screenEmpty);
    createProducers(numProds, producerInput, producerQueueArr, producerIds, producerQueueSizes, producerNumStrings);
    createDispatcher(dispInput, producerQueueArr, dispatcherQueueArr, numProds);
    createCoEditors(coEditorInputs, dispatcherQueueArr, editorsQueue);
    createScreenManager(managerInputs, editorsQueue, totalStrings);
    
    joinThreads(numProds);

    // Clean up
    cleanUpProducers(numProds, producerQueueArr, producerInput);
    cleanUpDispatcher(dispatcherQueueArr);
    cleanUpScreenManager(editorsQueue, managerInputs);
    free(producersArr);
    free(producerAttrs);
    free(dispInput);
    for (int i = 0; i < NUM_CO_EDITORS; i++) {
        free(coEditorInputs[i]);
    }

    return 0;
}