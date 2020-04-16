#ifndef __philosophers_H
#define __philosophers_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "PL011.h"

#include "libc.h"
#include "philosopherProcess.h"
#include "mutex.h"

#define NUM_PHIL 16

#define FREE 0
#define TAKEN 1

typedef struct {
       pid_t   pid; // Process IDentifier (PID)
       int     status; // current status
       mutex_t*  left; // left fork
       mutex_t*  right; // right fork
} phil_t;

extern mutex_t forks[NUM_PHIL];

extern phil_t philosophers[NUM_PHIL];

#endif
