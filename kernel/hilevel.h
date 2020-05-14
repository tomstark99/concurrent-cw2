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

#include "PL050.h"
#include "PL111.h"
#include   "SYS.h"

#include "font.h"
#include "PS2.h"

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

#define WIDTH 800
#define HEIGHT 600

typedef int pid_t;

typedef enum {
  WHITE = 0x7FFF,
  BLACK = 0x0000,
  RED   = 0x001F,
  GREEN = 0x03E0,
  BLUE  = 0x7C00
} colour_t;

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
     char *  prog;
} pcb_t;

typedef struct {
    int x;
    int y;
    int scale;
    int x_max;
    int x_min;
    int y_max;
    int y_min;
} mouse_t;

#define NUM_WORDS 19

typedef struct {
    char   *word;
    int   length;
    int        x;
    int        y;
    int    scale;
    colour_t neg;
    colour_t pos;
} word_t;

word_t words[NUM_WORDS] = {
  {"tom", 3, 300, 20, 3, BLACK, WHITE},
  {"OS", 2, 380, 12, 4, RED, WHITE},
  {"press the corresponding key for the program", 43, 30, 70, 2, BLACK, GREEN},
  {"you would like to execute", 25, 30, 100, 2, BLACK, GREEN},
  {"3", 1, 150, 140, 3, BLUE, WHITE},
  {"4", 1, 150, 180, 3, BLUE, WHITE},
  {"5", 1, 150, 220, 3, BLUE, WHITE},
  {"P", 1, 150, 260, 3, BLUE, WHITE},
  {"to run P3", 9, 200, 140, 2, BLACK, WHITE},
  {"to run P4", 9, 200, 180, 2, BLACK, WHITE},
  {"to run P5", 9, 200, 220, 2, BLACK, WHITE},
  {"to run philosophers", 19, 200, 260, 2, BLACK, WHITE},
  {"these are the list of processes and the", 39, 30, 300, 2, BLACK, GREEN},
  {"program that are currently running", 34, 30, 330, 2, BLACK, GREEN},
  {"click", 5, 30, 570, 2, BLACK, GREEN},
  {"T", 1, 126, 562, 3, RED, WHITE},
  {"to terminate", 12, 166, 570, 2, BLACK, GREEN},
  {"ALL", 3, 374, 562, 3, RED, WHITE},
  {"processes and reset", 19, 462, 570, 2, BLACK, GREEN}
};

word_t procs[MAX_PROCS] = {
  {"1 ", 2, 40,  400, 2, RED, WHITE},
  {"2 ", 2, 80,  400, 2, RED, WHITE},
  {"3 ", 2, 120, 400, 2, RED, WHITE},
  {"4 ", 2, 160, 400, 2, RED, WHITE},
  {"5 ", 2, 200, 400, 2, RED, WHITE},
  {"6 ", 2, 240, 400, 2, RED, WHITE},
  {"7 ", 2, 280, 400, 2, RED, WHITE},
  {"8 ", 2, 320, 400, 2, RED, WHITE},
  {"9 ", 2, 360, 400, 2, RED, WHITE},
  {"10", 2, 400, 400, 2, RED, WHITE},
  {"11", 2, 440, 400, 2, RED, WHITE},
  {"12", 2, 480, 400, 2, RED, WHITE},
  {"13", 2, 520, 400, 2, RED, WHITE},
  {"14", 2, 560, 400, 2, RED, WHITE},
  {"15", 2, 600, 400, 2, RED, WHITE},
  {"16", 2, 640, 400, 2, RED, WHITE},
  {"17", 2, 40,  480, 2, RED, WHITE},
  {"18", 2, 80,  480, 2, RED, WHITE},
  {"19", 2, 120, 480, 2, RED, WHITE},
  {"20", 2, 160, 480, 2, RED, WHITE},
  {"21", 2, 200, 480, 2, RED, WHITE},
  {"22", 2, 240, 480, 2, RED, WHITE},
  {"23", 2, 280, 480, 2, RED, WHITE},
  {"24", 2, 320, 480, 2, RED, WHITE},
  {"25", 2, 360, 480, 2, RED, WHITE},
  {"26", 2, 400, 480, 2, RED, WHITE},
  {"27", 2, 440, 480, 2, RED, WHITE},
  {"28", 2, 480, 480, 2, RED, WHITE},
  {"29", 2, 520, 480, 2, RED, WHITE},
  {"30", 2, 560, 480, 2, RED, WHITE},
  {"31", 2, 600, 480, 2, RED, WHITE},
  {"32", 2, 640, 480, 2, RED, WHITE}
};

word_t exing[MAX_PROCS] = {
  {"  ", 2, 40,  420, 2, BLACK, WHITE},
  {"  ", 2, 80,  420, 2, BLACK, WHITE},
  {"  ", 2, 120, 420, 2, BLACK, WHITE},
  {"  ", 2, 160, 420, 2, BLACK, WHITE},
  {"  ", 2, 200, 420, 2, BLACK, WHITE},
  {"  ", 2, 240, 420, 2, BLACK, WHITE},
  {"  ", 2, 280, 420, 2, BLACK, WHITE},
  {"  ", 2, 320, 420, 2, BLACK, WHITE},
  {"  ", 2, 360, 420, 2, BLACK, WHITE},
  {"  ", 2, 400, 420, 2, BLACK, WHITE},
  {"  ", 2, 440, 420, 2, BLACK, WHITE},
  {"  ", 2, 480, 420, 2, BLACK, WHITE},
  {"  ", 2, 520, 420, 2, BLACK, WHITE},
  {"  ", 2, 560, 420, 2, BLACK, WHITE},
  {"  ", 2, 600, 420, 2, BLACK, WHITE},
  {"  ", 2, 640, 420, 2, BLACK, WHITE},
  {"  ", 2, 40,  500, 2, BLACK, WHITE},
  {"  ", 2, 80,  500, 2, BLACK, WHITE},
  {"  ", 2, 120, 500, 2, BLACK, WHITE},
  {"  ", 2, 160, 500, 2, BLACK, WHITE},
  {"  ", 2, 200, 500, 2, BLACK, WHITE},
  {"  ", 2, 240, 500, 2, BLACK, WHITE},
  {"  ", 2, 280, 500, 2, BLACK, WHITE},
  {"  ", 2, 320, 500, 2, BLACK, WHITE},
  {"  ", 2, 360, 500, 2, BLACK, WHITE},
  {"  ", 2, 400, 500, 2, BLACK, WHITE},
  {"  ", 2, 440, 500, 2, BLACK, WHITE},
  {"  ", 2, 480, 500, 2, BLACK, WHITE},
  {"  ", 2, 520, 500, 2, BLACK, WHITE},
  {"  ", 2, 560, 500, 2, BLACK, WHITE},
  {"  ", 2, 600, 500, 2, BLACK, WHITE},
  {"  ", 2, 640, 500, 2, BLACK, WHITE}
};

#endif
