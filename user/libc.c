/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of 
 * which can be found via http://creativecommons.org (and should be included as 
 * LICENSE.txt within the associated archive or repository).
 */

#include "libc.h"

int  atoi( char* x        ) {
  char* p = x; bool s = false; int r = 0;

  if     ( *p == '-' ) {
    s =  true; p++;
  }
  else if( *p == '+' ) {
    s = false; p++;
  }

  for( int i = 0; *p != '\x00'; i++, p++ ) {
    r = s ? ( r * 10 ) - ( *p - '0' ) :
            ( r * 10 ) + ( *p - '0' ) ;
  }

  return r;
}

void itoa( char* r, int x ) {
  char* p = r; int t, n;

  if( x < 0 ) {
     p++; t = -x; n = t;
  }
  else {
          t = +x; n = t;
  }

  do {
     p++;                    n /= 10;
  } while( n );

    *p-- = '\x00';

  do {
    *p-- = '0' + ( t % 10 ); t /= 10;
  } while( t );

  if( x < 0 ) {
    *p-- = '-';
  }

  return;
}

void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}

int write( int fd, const void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_WRITE), "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int  read( int fd,       void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r) 
              : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n) 
              : "r0", "r1", "r2" );

  return r;
}

int  fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0 
              : "=r" (r) 
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}

void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

void exec( const void* x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x)
              : "r0" );

  return;
}

int phil() {
  int r; // creates a variable r to store the register value (pid)
  asm volatile( "svc %1     \n" // make system call SYS_PHIL
                "mov %0, r0 \n" // assign r = r0
              : "=r" (r) 
              : "I" (SYS_PHIL)
              : "r0" );
  return r;
}

int  kill( int pid, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =  pid
                "mov r1, %3 \n" // assign r1 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r) 
              : "I" (SYS_KILL), "r" (pid), "r" (x)
              : "r0", "r1" );

  return r;
}

int kill_all( int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r) 
              : "I" (SYS_KILL_ALL), "r" (x)
              : "r0" );

  return r;
}

void nice( int pid, int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  pid
                "mov r1, %2 \n" // assign r1 =    x
                "svc %0     \n" // make system call SYS_NICE
              : 
              : "I" (SYS_NICE), "r" (pid), "r" (x)
              : "r0", "r1" );

  return;
}

int rand2( void ) {
  next = next * 234503515245 + 123456; // arbitrary manipulation of the seed
  return (unsigned int) next % 268435456; // making sure the random number stays within a certain limit so the wait time is not excessively long
}

void srand2(unsigned int seed) {
  next = seed;
  return;
}

void wait(unsigned int c) {
  for(int i = 0; i < c; i++) {
    asm volatile( "nop" ); // do no operation for as many times as c
  }
}

void sem_wait( void *s ) {
  asm volatile( "ldrex r1, [r0]     \n" // s’ = MEM[&s]           loads the memory address of the semaphore into r1
                "cmp r1, #0         \n" // s’ ?= 0                checks to see if the value fo the semaphore is equal to 0
                "beq sem_wait       \n" // if s’ == 0, retry      if it is 0 then the program is allowed to access the critical section
                "sub r1, r1, #1     \n" // s’ = s’ - 1            subtract 1 from the value of the semaphore
                "strex r2, r1, [r0] \n" // r <= MEM[&s] = s’      store the new value of the semaphore in the initial memory location
                "cmp r2, #0         \n" // r ?= 0                 compare the new value to 0
                "bne sem_wait       \n" // if r != 0, retry       if the new value isn't 0 then branch until it is until it can go into the critical section it is stuck in the wait
                "dmb                \n" // memory barrier         makes sure the above is observed before any memory is accessed
                "bx lr              \n"); // return
  return;
}

void sem_post( void *s ) {

  asm volatile( "ldrex r1, [r0]     \n" // s’ = MEM[&s]           loads the memory address of the semaphore into r1
                "add r1, r1, #1     \n" // s’ = s’ + 1            adds 1 to the value of the semaphore
                "strex r2, r1, [r0] \n" // r <= MEM[&s] = s’      stores the new value of the semaphore in the initial memory location
                "cmp r2, #0         \n" // r ?= 0                 checks if the new value is 0
                "bne sem_post       \n" // if r != 0, retry       if it is not equal to zero then the program is no longer allowed to access the critical section
                "dmb                \n" // memory barrier         makes sure the above is observed before any memory is accessed
                "bx lr              \n"); // return
  return;
}
