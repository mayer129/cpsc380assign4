# cpsc380assign4
Assignment 4: Using Threads and Mutex/Counting Semaphores
## Information
*  @author: Travis Mayer
*     ID: 002273275
*     Email: mayer129@mail.chapman.edu
*     Course: CPSC 380-01

## Program Purpose
"The objective of this assignment is to use semaphores to protect the critical section between two
competing threads."
## Steps to Compile & Run:
1. Path to the prodcon directory
2. type `gcc prodcon.c -o prodcon` to compile the program
3. type `./mcarlo <memsize (must be divisible by 32)> <ntimes>`
* This will compile the program. The program will take command line input in the form of the memsize which must be divisible by 32, and ntimes which is the number of times the producer will write and the consumer will read from the shared memory region.