#ifndef __philosophers_H
#define __philosophers_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "PL011.h"

#include "libc.h"

#define NUM_PHILOS 16

typedef enum {
    FREE,
    TAKEN
} status_t;

typedef struct {
     pid_t    pid; // Process IDentifier (PID)
  status_t status; // current status
       int   left; // left fork
       int  right; // right fork
} pcb_t;

#endif
