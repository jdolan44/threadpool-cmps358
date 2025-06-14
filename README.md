# Thread Pool implementation in C

This repository is from a class project for CMPS 358 (Real-Time Systems). This thread pool was built from scratch
to reinforce course concepts.

### What is a ThreadPool?

A Thread pool manages multiple threads in a program. Tasks are sent to the thread pool
and then distributed amongst a set number of threads for execution.

This implementaion uses a Blocking Queue, a queue with a set size that can cause other threads to block.
If a thread calls `enqueue` when the queue is full, it gets blocked until space is available.
if a thread calls `dequeue` when the queue is empty, it gets blocked until a new item is available.

### Running this project

This project was run in a docker container using the following command:

`docker run --rm -it -v ${pwd}:/usr/src/myapp -w /usr/src/myapp gcc:latest /bin/bash`

### Concepts learned

- Thread pools, blocking queues, and their usage.
- POSIX threads
- Managing race conditions with POSIX mutex.
- Thread Cancellation in C/POSIX
  - (learning cancellation points was a "trial by fire" to say the least... 😊)
- C Makefile
- Docker
