#include "philosophers.h"

// depending on which method is used to solve the IPC problem we need both methods
extern void main_PhilosopherMutex();
extern void main_PhilosopherSemaphore();
//const uint32_t locked = 0;

char s[10]; // used to output to the qemu which pid a philosopher has

void initialiseMutexes() {
    waiter = FREE; // set the waiter semaphore (FREE is 1 so the default semaphore value, would be 0 if we were using mutexes)
    for (int i = 0; i < NUM_PHIL; i++) {
        forks[i] = FREE; // set each fork semaphore to 1 (FREE)
    }
}

void runPhilosophers() {
    int cb = 0; // make a temp callback variable to be used as the reply of fork
    global_waiter = &waiter; // global waiter pointer to the address of the waiter
    for (int i = 0; i < NUM_PHIL; i++) {
        philosophers[i].left = &forks[(i) % NUM_PHIL]; //give each philosopher a left and a right fork corresponding to their position
        philosophers[i].right = &forks[(i + 1) % NUM_PHIL];
        cb = fork(); // call fork 16 times to spawn 16 processes
        if(cb != 0) {
            philosophers[i].pid = cb; // set the pid to the process pid which will be useful later
        }
        else if( cb == 0 ) {

            exec(&main_PhilosopherMutex); // exec each philosopher with the mutex implementation, I changed the default locked and unlocked values for the mutexes so that you did not have to change the definition of free if you want to switch between them.
            //exec(&main_PhilosopherSemaphore); // exec each philosopher with the semaphore implementation
            
        }
    }
}

void main_Philosophers() {
    initialiseMutexes(); // function called in this order is important
    runPhilosophers();
    exit(EXIT_SUCCESS); // exit with success
}
