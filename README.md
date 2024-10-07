# Producer-Consumer: Concurrent Programming and Synchronization Mechanisms Assignment

## General Description
The aim of this assignment is to develop skills in concurrent programming and synchronization mechanisms. It's crucial to design the solution carefully before starting the implementation. Otherwise, the implementation may be very challenging.

This scenario also helps understand the performance impact of various synchronization constructs under different loads.

The scenario we're simulating is news broadcasting. Different types of stories are produced, and the system sorts and displays them to the public.

In this assignment, 'news stories' are represented by simple strings that should be displayed on the screen in the order they arrive.

In the scenario you should implement, there are 4 types of active participants:
## Producer
Each producer creates several strings in the following format:
```bash
“producer <i> <type> <j>”
```
- ```<i>```: Producer's ID
- ```<type>```: A random type chosen from SPORTS, NEWS, WEATHER
- ```<j>```: The number of strings of type <type> this producer has already produced

The number of products a producer makes is specified via its constructor.

For example, if producer 2 needs to create 3 strings, a possible outcome could be:
```
Producer 2 SPORTS 0
Producer 2 SPORTS 1
Producer 2 WEATHER 0
```
Each producer sends its information to the Dispatcher (introduced below) via its own private queue. Each producer's private queue is shared between the Producer and the Dispatcher. Each of the string products is inserted by the Producer into its 'producer queue'. After inserting all the products, the Producer sends a 'DONE' string through its queue.
