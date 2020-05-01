#ifndef __philosophers_H
#define __philosophers_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "PL011.h"

#include "libc.h"

#define NUM_PHIL 16

#define FREE 1 // the initial value of the semaphores (the initial value of the mutexes used to be 0, for unlocked and 1 for locked but I changed this around to make it easier to switch between the two)

#define SUCCESS 10
#define FAIL -1 

#define THINKING 2 // enums for the states of the philosophers
#define EATING 3
#define HUNGRY 4

typedef struct {
       pid_t    pid; // Process IDentifier (PID)
       uint32_t state; // current state
       uint32_t eaten; // how many times a philosopher has eaten, aims to help avoid starvation
       sem_t*   left; // left fork a pointer to a fork in the fork array
       sem_t*   right; // right fork a pointer to a fork in the fork array
} phil_t;

sem_t waiter;
sem_t* global_waiter;
sem_t forks[NUM_PHIL]; // array of semaphores for the forks a philosopher needs to eat
phil_t philosophers[NUM_PHIL];

#endif
