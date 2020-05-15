/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#ifndef __LIBC_H
#define __LIBC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <mutex.h>

// Define a type that that captures a Process IDentifier (PID).
typedef int pid_t;

// define a type that captures a semaphore (also used for mutex) this allows for generic IPC using semaphores
typedef uint32_t sem_t;

// the variable used by the random function to store the seed 
unsigned int next;

/* The definitions below capture symbolic constants within these classes:
 *
 * 1. system call identifiers (i.e., the constant used by a system call
 *    to specify which action the kernel should take),
 * 2. signal identifiers (as used by the kill system call), 
 * 3. status codes for exit,
 * 4. standard file descriptors (e.g., for read and write system calls),
 * 5. platform-specific constants, which may need calibration (wrt. the
 *    underlying hardware QEMU is executed on).
 *
 * They don't *precisely* match the standard C library, but are intended
 * to act as a limited model of similar concepts.
 */

#define SYS_YIELD     ( 0x00 )
#define SYS_WRITE     ( 0x01 )
#define SYS_READ      ( 0x02 )
#define SYS_FORK      ( 0x03 )
#define SYS_EXIT      ( 0x04 )
#define SYS_EXEC      ( 0x05 )
#define SYS_KILL      ( 0x06 )
#define SYS_NICE      ( 0x07 )
// a system call to get the pid and therefore the philosopher number for indexing the philosopher in the IPC solution
#define SYS_PHIL      ( 0x08 )
// a system call for killing all processes
#define SYS_KILL_ALL  ( 0x09 )
// a system call for assigning a program descriptor to a process
#define SYS_DRAW      ( 0x10 )

#define SIG_TERM      ( 0x00 )
#define SIG_QUIT      ( 0x01 )

#define EXIT_SUCCESS  ( 0 )
#define EXIT_FAILURE  ( 1 )

#define  STDIN_FILENO ( 0 )
#define STDOUT_FILENO ( 1 )
#define STDERR_FILENO ( 2 )

// convert ASCII string x into integer r
extern int  atoi( char* x        );
// convert integer x into ASCII string r
extern void itoa( char* r, int x );

// cooperatively yield control of processor, i.e., invoke the scheduler
extern void yield();
// write n bytes from x to   the file descriptor fd; return bytes written
extern int write( int fd, const void* x, size_t n );
// read  n bytes into x from the file descriptor fd; return bytes read
extern int  read( int fd,       void* x, size_t n );
// wait for given counter
// perform fork, returning 0 iff. child or > 0 iff. parent process
extern int  fork();
// perform exit, i.e., terminate process with status x
extern void exit(       int   x );
// perform exec, i.e., start executing program at address x
extern void exec( const void* x );
// for process identified by pid, send signal of x
extern int  kill( pid_t pid, int x );
// for process identified by pid, set  priority to x
extern void nice( pid_t pid, int x );

// ================= for philosophers =====================

// a custom random function that uses a seed based off the timer that runs in the kernel
extern int rand2();
// assigns the seed to the variable used in the random function
extern void srand2(unsigned int seed);
// a function that waits for a given processer tick count
extern void wait( unsigned int c );
// sem wait and sem post functions
extern void sem_wait( void *s );
extern void sem_post( void *s );
// perform an interrupt to get the pid for the currently executing process
extern int phil();

// ====================== for LCD =========================

extern void draw( int x );
extern int  kill_all( int x );


#endif
