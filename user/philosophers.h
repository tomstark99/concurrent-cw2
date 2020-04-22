#ifndef __philosophers_H
#define __philosophers_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "PL011.h"

#include "libc.h"

#define NUM_PHIL 16

#define FREE 1
//#define TAKEN 1

#define SUCCESS 10
#define FAIL -1

#define THINKING 2
#define EATING 3
#define HUNGRY 4

typedef uint32_t sem_t;

typedef struct {
       pid_t    pid; // Process IDentifier (PID)
       uint32_t state; // current state
       sem_t*   left; // left fork
       sem_t*   right; // right fork
} phil_t;

sem_t waiter;
sem_t* global_waiter;
sem_t forks[NUM_PHIL];
phil_t philosophers[NUM_PHIL];

#endif
