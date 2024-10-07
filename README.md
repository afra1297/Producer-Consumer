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
