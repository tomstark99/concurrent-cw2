/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#include "hilevel.h"

/* We assume there will be two user processes, stemming from execution of the
 * two user programs P1 and P2, and can therefore
 *
 * - allocate a fixed-size process table (of PCBs), and then maintain an index
 *   into it to keep track of the currently executing process, and
 * - employ a fixed-case of round-robin scheduling: no more processes can be
 *   created, and neither can be terminated, so assume both are always ready
 *   to execute.
 */

pcb_t procTab[ MAX_PROCS ]; pcb_t* executing = NULL; int num_procs = 0;

void dispatch( ctx_t* ctx, pcb_t* prev, pcb_t* next ) {
  char prev_pid = '?', next_pid = '?';

  if( NULL != prev ) {
    memcpy( &prev->ctx, ctx, sizeof( ctx_t ) ); // preserve execution context of P_{prev}
    prev_pid = '0' + prev->pid;
  }
  if( NULL != next ) {
    memcpy( ctx, &next->ctx, sizeof( ctx_t ) ); // restore  execution context of P_{next}
    next_pid = '0' + next->pid;
  }

    PL011_putc( UART0, '[',      true );
    PL011_putc( UART0, prev_pid, true );
    PL011_putc( UART0, '-',      true );
    PL011_putc( UART0, '>',      true );
    PL011_putc( UART0, next_pid, true );
    PL011_putc( UART0, ']',      true );

    executing = next;                           // update   executing process to P_{next}

  return;
}

int getChildProcess(){
  for(int i = 0; i < MAX_PROCS; i++) {
    if(procTab[i].status == STATUS_TERMINATED) {
      return i;
    }
  }
}

int getPriority() {
  int priorityIndex;
  int maxP = 0;
  for(int i = 0; i < MAX_PROCS; i++) {
    int tempP = 0;
    if(executing->pid != procTab[i].pid){
      tempP = procTab[i].priority + procTab[i].age;
    }
    if(maxP <= tempP && procTab[i].status != STATUS_TERMINATED) {
      maxP = tempP;
      priorityIndex = i;
    }
  }
  for(int i = 0; i < MAX_PROCS; i++) {
    if(procTab[i].status != STATUS_TERMINATED) {
      procTab[i].age = procTab[i].age + 1;
    }
  }
  procTab[priorityIndex].age = 0;

  return priorityIndex;
}

void schedule( ctx_t* ctx ) {
  int priority = getPriority();

  dispatch( ctx, executing, &procTab[ priority ] );
  executing->status = STATUS_READY;
  procTab[priority].status = STATUS_EXECUTING;

    // PL011_putc( UART0, '[',      true );
    // PL011_putc( UART0, '0' + executing->pid, true );
    // PL011_putc( UART0, '-',      true );
    // PL011_putc( UART0, 'R',      true );
    // PL011_putc( UART0, ']',      true );
    // PL011_putc( UART0, '[',      true );
    // PL011_putc( UART0, '0' + procTab[priority].pid, true );
    // PL011_putc( UART0, '-',      true );
    // PL011_putc( UART0, 'E',      true );
    // PL011_putc( UART0, ']',      true );
}
  
  // else if( executing->pid == procTab[ 2 ].pid ) {
  //   dispatch( ctx, &procTab[ 2 ], &procTab[ 0 ] );  // context switch P_2 -> P_1

  //   procTab[ 2 ].status = STATUS_READY;             // update   execution status  of P_2
  //   procTab[ 0 ].status = STATUS_EXECUTING;         // update   execution status  of P_1
  // }

extern void     main_console();
extern uint32_t tos_user;
extern void     main_P4();
// extern uint32_t tos_P4;
// extern void     main_P5();
// extern uint32_t tos_P5;

void hilevel_handler_rst( ctx_t* ctx              ) {
  /* Invalidate all entries in the process table, so it's clear they are not
   * representing valid (i.e., active) processes.
   */

    PL011_putc( UART0, '[',      true );
    PL011_putc( UART0, 'R', true );
    PL011_putc( UART0, 'E',      true );
    PL011_putc( UART0, 'S',      true );
    PL011_putc( UART0, 'E', true );
    PL011_putc( UART0, 'T',      true );
    PL011_putc( UART0, ']',      true );

  /* Automatically execute the user programs P1 and P2 by setting the fields
   * in two associated PCBs.  Note in each case that
   *
   * - the CPSR value of 0x50 means the processor is switched into USR mode,
   *   with IRQ interrupts enabled, and
   * - the PC and SP values match the entry point and top of stack.
   */

  memset( &procTab[ 0 ], 0, sizeof( pcb_t ) ); // initialise 0-th PCB = P_1
  procTab[ 0 ].pid      = 1;
  procTab[ 0 ].status   = STATUS_READY;
  procTab[ 0 ].tos      = ( uint32_t )( &tos_user  );
  procTab[ 0 ].ctx.cpsr = 0x50;
  procTab[ 0 ].ctx.pc   = ( uint32_t )( &main_console );
  procTab[ 0 ].ctx.sp   = procTab[ 0 ].tos;
  procTab[ 0 ].priority = 1;
  procTab[ 0 ].age      = 0;
  num_procs+=1;

  for (int i = 1; i < MAX_PROCS; i++) {
    memset( &procTab[ i ], 0, sizeof( pcb_t ) ); // initialise 1-st PCB = P_2
    procTab[ i ].pid      = i+1;
    procTab[ i ].status   = STATUS_TERMINATED;
    procTab[ i ].ctx.cpsr = 0x50;
    procTab[ i ].ctx.pc   = ( uint32_t )( &main_console );
    procTab[ i ].ctx.sp   = procTab[ 0 ].tos - (i*0x00001000);
    procTab[ i ].tos      = procTab[ i ].ctx.sp;
    procTab[ i ].priority = 1;
    procTab[ i ].age      = 0;
  }


  /* Once the PCBs are initialised, we arbitrarily select the 0-th PCB to be
   * executed: there is no need to preserve the execution context, since it
   * is invalid on reset (i.e., no process was previously executing).
   */

  dispatch( ctx, NULL, &procTab[ 0 ] );

  TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor

  int_enable_irq();

  return;
}

void hilevel_handler_svc( ctx_t* ctx, uint32_t id ) {
  /* Based on the identifier (i.e., the immediate operand) extracted from the
   * svc instruction,
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call, then
   * - write any return value back to preserved usr mode registers.
   */

  switch( id ) {
    case 0x00 : { // 0x00 => yield()
      schedule( ctx );

      break;
    }

    case 0x01 : { // 0x01 => write( fd, x, n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );
      char*  x = ( char* )( ctx->gpr[ 1 ] );
      int    n = ( int   )( ctx->gpr[ 2 ] );

      for( int i = 0; i < n; i++ ) {
        PL011_putc( UART0, *x++, true );
      }

      ctx->gpr[ 0 ] = n;

      break;
    }

    case 0x02 : { // 0x02 => read(int fd, void* x, size_t n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );
      char*  x = ( char* )( ctx->gpr[ 1 ] );
      int    n = ( int   )( ctx->gpr[ 2 ] );

      for( int i = 0; i < n; i++ ) {
        PL011_getc( UART0, true );
      }
      ctx->gpr[ 0 ] = n;

      break;
    }

    case 0x03 : { // fork
      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'F', true );
      PL011_putc( UART0, 'O', true );
      PL011_putc( UART0, 'R', true );
      PL011_putc( UART0, 'K', true );
      PL011_putc( UART0, ']', true );
      int childProcess = getChildProcess();
      pcb_t * child = &procTab[childProcess];

      if(child == NULL) {
        ctx->gpr[ 0 ] = child->pid;
        break;
      }


      memcpy(&child->ctx, ctx, sizeof(ctx_t));

      child->status = STATUS_CREATED;

      child->ctx.gpr[ 0 ] = 0;
      ctx->gpr[ 0 ] = child->pid;
      break;
    }

    case 0x04 : { // exit
      executing->status = STATUS_TERMINATED;
      schedule( ctx );
      break;
    }

    case 0x05 : { // exec

      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'E', true );
      PL011_putc( UART0, 'X', true );
      PL011_putc( UART0, 'E', true );
      PL011_putc( UART0, 'C', true );
      PL011_putc( UART0, ']', true );

      ctx->pc = ctx->gpr[ 0 ];

      break;
    }

    case 0x06 : { // kill

      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'K', true );
      PL011_putc( UART0, 'I', true );
      PL011_putc( UART0, 'L', true );
      PL011_putc( UART0, 'L', true );
      PL011_putc( UART0, ctx->gpr[0], true );
      PL011_putc( UART0, '-', true );
      PL011_putc( UART0, ctx->gpr[1], true );
      PL011_putc( UART0, ']', true );

      for(int i = 0; i < MAX_PROCS; i++) {
        if(procTab[i].pid == ctx->gpr[0]) {
          PL011_putc( UART0, 'T', true );
          procTab[i].ctx.pc = (uint32_t) (&main_console);
          procTab[i].status = STATUS_TERMINATED;
          schedule(ctx);
        }
      }
      
      break;
    }

    default   : { // 0x?? => unknown/unsupported
      break;
    }
  }

  return;
}

void hilevel_handler_irq(ctx_t* ctx) {
  // Step 2: read  the interrupt identifier so we know the source.

  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.

  if( id == GIC_SOURCE_TIMER0 ) {
    //PL011_putc( UART0, 'T', true );
    TIMER0->Timer1IntClr = 0x01;
    schedule(ctx);

  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;

  return;
}
