#ifndef __philosophers_H
#define __philosophers_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "PL011.h"

#include "libc.h"

#define NUM_PHIL 16

#define FREE 0
//#define TAKEN 1

#define SUCCESS 10
#define FAIL -1

#define THINKING 2
#define EATING 3

typedef uint32_t sem_t;

typedef struct {
       pid_t   pid; // Process IDentifier (PID)
       int     status; // current status
       sem_t*  left; // left fork
       sem_t*  right; // right fork
} phil_t;

//sem_t waiter = FREE;
sem_t forks[NUM_PHIL];
phil_t philosophers[NUM_PHIL];

#endif
