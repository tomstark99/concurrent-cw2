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

// include device drivers

#include "PS2.h"

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include     "int.h"
#include    "font.h"

// include functionality for srand

#include "libc.h"

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
#define BASE_PRIORITY 1 // the base priority for the processes

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
     char *  prog;
} pcb_t;

// width and height of the LCD
#define WIDTH 800
#define HEIGHT 600

// colours available to use for the LCD
typedef enum {
  WHITE = 0x7FFF,
  BLACK = 0x0000,
  RED   = 0x001F,
  GREEN = 0x03E0,
  BLUE  = 0x7C00
} colour_t;

typedef struct {
    int x;     // current x coordinate of the mouse
    int y;     // current y coordinate of the mouse
    int scale; // the scale the mouse should be drawn at
    int x_max; // the max x value the mouse can have (dependent on the screen size)
    int x_min; // the min x value the mouse can have
    int y_max; // the max y value the mouse can have
    int y_min; // the mim y value the mouse can have
} mouse_t;

#define NUM_WORDS 23 // the number of word_t 'sentences' that need to be drawn

typedef struct {
    char   *word; // the sentence to draw
    int   length; // the length of the sentence
    int        x; // the top left x coordinate to start drawing from
    int        y; // the top left y coordiante to start drawing from
    int    scale; // the scale to draw the text at e.g. 2 would be double 8x8
    colour_t neg; // the negative colour of the 8x8 block
    colour_t pos; // the positive colour of the 8x8 block
} word_t;

// an array of all words on the screen to draw
word_t words[NUM_WORDS] = {
  {"X", 1, 756, 20, 3, BLUE, WHITE},
  {"tom", 3, 320, 20, 3, BLACK, WHITE},
  {"OS", 2, 400, 12, 4, RED, WHITE},
  {"press the corresponding key for the program", 43, 40, 70, 2, BLACK, GREEN},
  {"you would like to execute", 25, 40, 100, 2, BLACK, GREEN},
  {"3", 1, 150, 140, 3, BLUE, WHITE},
  {"4", 1, 150, 180, 3, BLUE, WHITE},
  {"5", 1, 150, 220, 3, BLUE, WHITE},
  {"P", 1, 150, 260, 3, BLUE, WHITE},
  {"to run P3", 9, 200, 140, 2, BLACK, WHITE},
  {"to run P4", 9, 200, 180, 2, BLACK, WHITE},
  {"to run P5", 9, 200, 220, 2, BLACK, WHITE},
  {"to run philosophers", 19, 200, 260, 2, BLACK, WHITE},
  {"these are the list of processes and the", 39, 40, 300, 2, BLACK, GREEN},
  {"program that are currently running", 34, 40, 330, 2, BLACK, GREEN},
  {"press", 5, 40, 540, 2, BLACK, GREEN},
  {"T", 1, 136, 540, 2, BLUE, WHITE},
  {"or click", 8, 168, 540, 2, BLACK, GREEN},
  {"X", 1, 312, 540, 2, BLUE, WHITE},
  {"to terminate", 12, 344, 540, 2, BLACK, GREEN},
  {"ALL", 3, 558, 540, 2, BLACK, RED},
  {"processes", 9, 622, 540, 2, BLACK, GREEN},
  {"and reset", 9, 40, 570, 2, BLACK, GREEN}
};

// an array of all processes to draw on the screen
word_t procs[MAX_PROCS] = {
  {"1 ", 2, 40,  380, 2, RED, WHITE},
  {"2 ", 2, 80,  380, 2, RED, WHITE},
  {"3 ", 2, 120, 380, 2, RED, WHITE},
  {"4 ", 2, 160, 380, 2, RED, WHITE},
  {"5 ", 2, 200, 380, 2, RED, WHITE},
  {"6 ", 2, 240, 380, 2, RED, WHITE},
  {"7 ", 2, 280, 380, 2, RED, WHITE},
  {"8 ", 2, 320, 380, 2, RED, WHITE},
  {"9 ", 2, 360, 380, 2, RED, WHITE},
  {"10", 2, 400, 380, 2, RED, WHITE},
  {"11", 2, 440, 380, 2, RED, WHITE},
  {"12", 2, 480, 380, 2, RED, WHITE},
  {"13", 2, 520, 380, 2, RED, WHITE},
  {"14", 2, 560, 380, 2, RED, WHITE},
  {"15", 2, 600, 380, 2, RED, WHITE},
  {"16", 2, 640, 380, 2, RED, WHITE},
  {"17", 2, 40,  460, 2, RED, WHITE},
  {"18", 2, 80,  460, 2, RED, WHITE},
  {"19", 2, 120, 460, 2, RED, WHITE},
  {"20", 2, 160, 460, 2, RED, WHITE},
  {"21", 2, 200, 460, 2, RED, WHITE},
  {"22", 2, 240, 460, 2, RED, WHITE},
  {"23", 2, 280, 460, 2, RED, WHITE},
  {"24", 2, 320, 460, 2, RED, WHITE},
  {"25", 2, 360, 460, 2, RED, WHITE},
  {"26", 2, 400, 460, 2, RED, WHITE},
  {"27", 2, 440, 460, 2, RED, WHITE},
  {"28", 2, 480, 460, 2, RED, WHITE},
  {"29", 2, 520, 460, 2, RED, WHITE},
  {"30", 2, 560, 460, 2, RED, WHITE},
  {"31", 2, 600, 460, 2, RED, WHITE},
  {"32", 2, 640, 460, 2, RED, WHITE}
};

// an array of all the programs being executed by the processes to be drawn
word_t exing[MAX_PROCS] = {
  {"  ", 2, 40,  400, 2, BLACK, WHITE},
  {"  ", 2, 80,  400, 2, BLACK, WHITE},
  {"  ", 2, 120, 400, 2, BLACK, WHITE},
  {"  ", 2, 160, 400, 2, BLACK, WHITE},
  {"  ", 2, 200, 400, 2, BLACK, WHITE},
  {"  ", 2, 240, 400, 2, BLACK, WHITE},
  {"  ", 2, 280, 400, 2, BLACK, WHITE},
  {"  ", 2, 320, 400, 2, BLACK, WHITE},
  {"  ", 2, 360, 400, 2, BLACK, WHITE},
  {"  ", 2, 400, 400, 2, BLACK, WHITE},
  {"  ", 2, 440, 400, 2, BLACK, WHITE},
  {"  ", 2, 480, 400, 2, BLACK, WHITE},
  {"  ", 2, 520, 400, 2, BLACK, WHITE},
  {"  ", 2, 560, 400, 2, BLACK, WHITE},
  {"  ", 2, 600, 400, 2, BLACK, WHITE},
  {"  ", 2, 640, 400, 2, BLACK, WHITE},
  {"  ", 2, 40,  480, 2, BLACK, WHITE},
  {"  ", 2, 80,  480, 2, BLACK, WHITE},
  {"  ", 2, 120, 480, 2, BLACK, WHITE},
  {"  ", 2, 160, 480, 2, BLACK, WHITE},
  {"  ", 2, 200, 480, 2, BLACK, WHITE},
  {"  ", 2, 240, 480, 2, BLACK, WHITE},
  {"  ", 2, 280, 480, 2, BLACK, WHITE},
  {"  ", 2, 320, 480, 2, BLACK, WHITE},
  {"  ", 2, 360, 480, 2, BLACK, WHITE},
  {"  ", 2, 400, 480, 2, BLACK, WHITE},
  {"  ", 2, 440, 480, 2, BLACK, WHITE},
  {"  ", 2, 480, 480, 2, BLACK, WHITE},
  {"  ", 2, 520, 480, 2, BLACK, WHITE},
  {"  ", 2, 560, 480, 2, BLACK, WHITE},
  {"  ", 2, 600, 480, 2, BLACK, WHITE},
  {"  ", 2, 640, 480, 2, BLACK, WHITE}
};

#endif
