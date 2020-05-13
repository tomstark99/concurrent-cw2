/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#ifndef __HILEVEL_H
#define __HILEVEL_H

// Include functionality relating to newlib (the standard C library).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

// Include functionality relating to the platform.

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"
#include "disk.h"

#include "PL050.h"
#include "PL111.h"
#include   "SYS.h"

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include     "int.h"
#include "philosophers.h"

/* The kernel source code is made simpler and more consistent by using
 * some human-readable type definitions:
 *
 * - a type that captures a Process IDentifier (PID), which is really
 *   just an integer,
 * - an enumerated type that captures the status of a process, e.g.,
 *   whether it is currently executing,
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
 *   preservation and restoration prologue and epilogue, and
 * - a type that captures a process PCB.
 */

#define MAX_PROCS 32 // 32 seemed like a good number for max processes e.g. because of the IPC mechanism showcase taking up 16 giving a comfortable amount of processes left for different programs
#define BASE_PRIORITY 1 // defined but not used

typedef int pid_t;

typedef enum {
  STATUS_INVALID,

  STATUS_CREATED, // child process is set to this when fork is called
  STATUS_TERMINATED, // process initially set to this when created in rst

  STATUS_READY, // process status when has been created and after it has been executed it becomes ready
  STATUS_EXECUTING, // only one process should have this at any one time, when it is currently executing
  STATUS_WAITING
} status_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef struct {
     pid_t    pid; // Process IDentifier (PID)
  status_t status; // current status
  uint32_t    tos; // address of Top of Stack (ToS)
     ctx_t    ctx; // execution context
     pid_t    age;    // I added age and priority to the pcb struct so they could be easily accessed from within the kernel for the scheduling of the processes
     pid_t priority;
} pcb_t;

#endif
