#include "Producer.h"

// Define the options for string types
const char* types[] = {"SPORTS", "NEWS", "WEATHER"};

// Function to create strings and input them into the Bounded Buffer
void * produceStrings(void *input) {
    // Parse the inputs given to the function
    ProducerInput *inputs = (ProducerInput *)input;
    int numProducer = inputs->id;
    int numStrings = inputs->numStrings;
    BoundedBuffer *buffer = inputs->buffer;

    // Initalize counters and random seed
    int numNews = 0, numSports = 0, numWeather = 0;
    srand(time(NULL));

    // Add strings to the buffer
    for (int i = 0; i < numStrings; i++) {
        int randIndex = rand() % 3;
        int *numToIncrement;
        // Select the correct number to increment
        switch(randIndex) {
            case 0: // SPORTS
                numToIncrement = &numSports;
                break;
            case 1: // NEWS
                numToIncrement = &numNews;
                break;
            case 2: // WEATHER
                numToIncrement = &numWeather;
                break;
            default:
        }

        char *str = (char *)malloc(sizeof(char)*MAX_PRODUCER_STRINGLEN);
        if (str == NULL) {
            exit(1);
        }
        sprintf(str, "Producer %d %s %d", numProducer, types[randIndex], *numToIncrement);
        boundedInsert(buffer, str);
        *numToIncrement = *numToIncrement + 1;
    }
    // Indicate that done
    boundedInsert(buffer, "DONE");
    return NULL;
}