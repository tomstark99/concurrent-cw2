/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#include "hilevel.h"

/* We assume there will be n user processes, and therefore are uncertain how many
 * we will have, this means: 
 *
 * - allocate a variable-size process table (of PCBs), and then maintain an index
 *   into it to keep track of the currently executing process, and
 * - employ a priority scheduling algorithm: processes can be created and terminated
 *   but only through the console program so we must make sure this process is always
 *   ready so it can be scheduled
 */

/* - creates an array processes of the maximum size specified in hilevel.h
 * - creates a variable to store a pointer to the currently executing process
 * - creates a variable to keep track of the active number of processes
 */
pcb_t procTab[ MAX_PROCS ]; pcb_t* executing = NULL; int num_procs = 0;

// dispatch changes the executing processes from the previous to the next which is given by the schedule function
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

// this function returns a pointer to the next 'available' process that has been marked as not in use
pcb_t* getChildProcess(){
  for(int i = 0; i < MAX_PROCS; i++) { // loop through all the available processes
    if(procTab[i].status == STATUS_TERMINATED) {
      return &procTab[i]; // if a process matches a flag of not being in use then return it as a child
    }
  }
  return NULL; // if no process if found then return null which means that a fork can not be executed
}

// this is the function that returns the index of the process that has the highest priority (is to be executed next)
int getPriority() {
  int priorityIndex; // a variable that changes in the for loop if a certain condition is met leaving the value we want
  int maxP = 0;
  for(int i = 0; i < MAX_PROCS; i++) {
    int tempP = 0; // set the per turn priority to zero
    if(executing->pid != procTab[i].pid){ // check that the process you are checking is not the one that is currently executing as you do not want to execute that one again
      tempP = procTab[i].priority + procTab[i].age; // use the priority formula to get the priority for that process
    } // check if the priority for that process is larger or equal to the largest priority we have seen so far
    if(maxP <= tempP && procTab[i].status != STATUS_TERMINATED) { // also make sure we dont schedule a process that is terminated
      maxP = tempP; // change the new max priority
      priorityIndex = i; // change the potential index of the process to be scheduled
    }
  }
  for(int i = 0; i < MAX_PROCS; i++) { // all the processes need to age per round otherwise their priority will always be the same
    if(procTab[i].status != STATUS_TERMINATED) { // once again making sure to not include processes that can't be scheduled
      procTab[i].age = procTab[i].age + 1;
    }
  }
  procTab[priorityIndex].age = 0; // set the age of the process that is about to be executed to zero meaning when it is no longer executing it is at the back of the 'queue'

  return priorityIndex; // return the index that we have found
}

// this is the function that schedules a process to be executed (i.e. calls dispatch on it)
void schedule( ctx_t* ctx ) {
  int priority = getPriority(); // using the function above get the index of the process to be scheduled

  dispatch( ctx, executing, &procTab[ priority ] ); // dispatch the address of the new process to be changed, giving it the 'new' context that is passed into the schedule function as well as the currently executing process
  executing->status = STATUS_READY; // change the status of the currently executing process to ready (so it can be selected again from the priority function)
  procTab[priority].status = STATUS_EXECUTING; // change the priority of the now executing process to executing
}

// create a reference to the program that we want to load into the process (so it can be used in this file)
extern void     main_console();
// create a reference to the top of stack for the 'user' programs that the processes will run which is defined in image.ld
extern uint32_t tos_user;

void hilevel_handler_rst( ctx_t* ctx              ) {

    // this lets me or the user know if hilevel_handler_rst has been called from the qemu window
    PL011_putc( UART0, '[',      true );
    PL011_putc( UART0, 'R', true );
    PL011_putc( UART0, 'E',      true );
    PL011_putc( UART0, 'S',      true );
    PL011_putc( UART0, 'E', true );
    PL011_putc( UART0, 'T',      true );
    PL011_putc( UART0, ']',      true );

  /* - the CPSR value of 0x50 means the processor is switched into USR mode,
   *   with IRQ interrupts enabled, and
   * - the PC and SP values match the entry point and top of stack.
   */

  memset( &procTab[ 0 ], 0, sizeof( pcb_t ) ); // initialise 0-th PCB = console, so the user can interact with the kernel
  procTab[ 0 ].pid      = 1;
  procTab[ 0 ].status   = STATUS_READY;                    // the status of this should be ready so this one can be picked to be scheduled
  procTab[ 0 ].tos      = ( uint32_t )( &tos_user  );      // the top of stack for this program should be the top of the stack allocated in image.ld
  procTab[ 0 ].ctx.cpsr = 0x50;
  procTab[ 0 ].ctx.pc   = ( uint32_t )( &main_console );   // the program this process is executing is the main console 
  procTab[ 0 ].ctx.sp   = procTab[ 0 ].tos;                // the stack pointer for this program should be at the top of the stack of the process
  procTab[ 0 ].priority = 1;                               // the base priority for this and all processes is 1
  procTab[ 0 ].age      = 0;                               // the default age for this and all processes is 0
  num_procs+=1; // we are adding one to the number of active processes

  for (int i = 1; i < MAX_PROCS; i++) { // for all the rest of the processes we initialise them as dummy processes to be used at a point in execution
    memset( &procTab[ i ], 0, sizeof( pcb_t ) ); // initialise 1-st to MAX_PROCS-th PCB without setting the program as this will be done later when you execute one
    procTab[ i ].pid      = i+1;                                // the pid is an increasing number and unique for each process
    procTab[ i ].status   = STATUS_TERMINATED;                  // the status for these is terminated so we don't accidentally schedule them
    procTab[ i ].ctx.cpsr = 0x50;
    //procTab[ i ].ctx.pc   = ( uint32_t )( &main_console );    we dont allocate these processes a program to execute 
    procTab[ i ].ctx.sp   = procTab[ 0 ].tos - (i*0x00001000);  // the stack pointer for each of these is not the entry point for a program they are executing but instead the top of the stack allocated for that particular process, as the initial (0-th) process is right at the top we take away an increasing multiple of the stack space each process takes up (i.e. 0x00001000)
    procTab[ i ].tos      = procTab[ i ].ctx.sp;                // because we dont have an address for the top of stack for this process we set it to the stack pointer as this is the same
    procTab[ i ].priority = 1;                                  // priority and age are the same as the 0-th process
    procTab[ i ].age      = 0;
  }

  /* Once the PCBs are initialised, we arbitrarily select the 0-th PCB to be
   * executed: there is no need to preserve the execution context, since it
   * is invalid on reset (i.e., no process was previously executing).
   */

  dispatch( ctx, NULL, &procTab[ 0 ] );

  // this is the setup for the timer that controls the flow of execution in the kernel instead of program dependent yield calls
  TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor

  // this creates the seed for the random function in libc.c, I am using the timer value
  srand2(TIMER0->Timer1Value);

  // this starts the timer
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
    /* yield is not needed anymore, the way yield worked was a program
     * caused a kernel interrupt once it had finished executing
     * this kernel interrupt as you can see below caused the next process
     * to be scheduled (which also was executing a program that had a yield call)
     * because of this using yield to schedule more than 2 processes and programs
     * becomes tedious and tricky this makes the priority based scheduler and the
     * timer a much better solution as it does not rely on a specific program
     * being run
     */
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
    
    case 0x03 : { // fork aims to make a duplicate of a parent process called a child e.g. take one of the dummy processes and copy all of the execution state into it
      // informs the qemu the fork syscall was successful
      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'F', true );
      PL011_putc( UART0, 'O', true );
      PL011_putc( UART0, 'R', true );
      PL011_putc( UART0, 'K', true );
      PL011_putc( UART0, ']', true );
      
      pcb_t * child = getChildProcess(); // get the next available process that isn't currently being used

      // if no free process is found getChildProcess() returns NULL and the kernel breaks out of the interrupt
      if(child == NULL) {
        PL011_putc( UART0, '[', true );
        PL011_putc( UART0, 'N', true );
        PL011_putc( UART0, 'U', true );
        PL011_putc( UART0, 'L', true );
        PL011_putc( UART0, 'L', true );
        PL011_putc( UART0, ']', true );
        break;
      }
      // copy the context of the parent process into the context of the child remembering to allocate it in memory
      memcpy(&child->ctx, ctx, sizeof(ctx_t));

      // this offset is needed to set the stack pointer to the same point of execution as the parent, not setting it to the top of execution
      uint32_t offset = (uint32_t) executing->tos - ctx->sp;
      // copy the program that is being executed, because memcpy copies from bottom up, I need to take away the size of the stack for the process for it to copy the correct memory
      memcpy((void*) (child->tos - 0x00001000), (void*) (executing->tos - 0x00001000), 0x00001000);

      // change the status of the child process from terminated to something else (it isn't important what this is labelled as so I just chose created)
      child->status = STATUS_CREATED;
      child->ctx.sp = child->tos - offset; // set the stack pointer to the top of the stack minus the offset from the parent
      child->age = 0; // set the age to 0 although this should still be 0 from when the process that is now the child was created

      // add one to the active number of processes
      num_procs += 1;

      child->ctx.gpr[ 0 ] = 0; // set the return of the child process to 0
      ctx->gpr[ 0 ] = child->pid; // set the return of the parent process to the pid of the child
      break;
    }

    case 0x04 : { // exit
      // display on the qemu that an exit call was successful
      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'E', true );
      PL011_putc( UART0, 'X', true );
      PL011_putc( UART0, 'I', true );
      PL011_putc( UART0, 'T', true );
      PL011_putc( UART0, ']', true );
      // with exit we are just invalidating the process not completely removing all the execution context and therefore all we need to do is give it a terminated (not active) flag
      executing->status = STATUS_TERMINATED;
      num_procs -= 1; // remove one from the active number of processes
      schedule( ctx ); // schedule the next process because the current one is no longer useful
      break;
    }

    case 0x05 : { // exec
      // lets us know an exec call has been successful
      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'E', true );
      PL011_putc( UART0, 'X', true );
      PL011_putc( UART0, 'E', true );
      PL011_putc( UART0, 'C', true );
      PL011_putc( UART0, ']', true );

      ctx->sp = executing->tos; // the current executing process at the time of the exec call will be the child we set the stack pointer of the child to be the top of the stack for the new program
      ctx->pc = ctx->gpr[ 0 ]; // set the program from the address passed in from register 0
      //schedule(ctx);

      break;
    }

    case 0x06 : { // kill
      // notifies the qemu that a kill call has been successful
      PL011_putc( UART0, '[', true );
      PL011_putc( UART0, 'K', true );
      PL011_putc( UART0, 'I', true );
      PL011_putc( UART0, 'L', true );
      PL011_putc( UART0, 'L', true );
      PL011_putc( UART0, ']', true );

      uint32_t kill = (uint32_t) (ctx->gpr[ 0 ]) - 1; // get the pid that we want to kill from the register that was passed from the interrupt call makign sure to take one off so we can index it correctly
      if(procTab[ kill ].status != STATUS_TERMINATED) { 
        procTab[ kill ].status = STATUS_TERMINATED; // set the status of the process to 0
        //procTab[ kill ].ctx.pc = ( uint32_t )( &main_console ); we don't want more than one main console so leaving the pc alone waiting to be overwritten is the best solution // reset the program to the main console
        procTab[ kill ].ctx.sp = procTab[ 0 ].tos - (kill*0x00001000); // reset the stack pointer to the original value when the process was first created
        ctx->gpr[0] = 0; // a non essential return value of 0 is set to indicate the kill was successful
      }
      else {
        ctx->gpr[0] = -1; // a return value of -1 states the kill was unsuccessful
      }
      num_procs -= 1; // reduce the number of active processes;
      break;
    }

    case 0x08 : { // philosopher interrupt
      ctx->gpr[ 0 ] = executing->pid; // this interrupt is a custom system call to give the process pid of the currently executing philosopher so I can determine which philosopher process (in the philosopher array) is being executed, this is needed because the philosopher program execution is in a different file to the main file that spawns the 16 processes
      break;
    }

    default   : { // 0x?? => unknown/unsupported
      break;
    }
  }

  return;
}

// this is the function for handling the timer interrupt
void hilevel_handler_irq(ctx_t* ctx) {
  // Step 2: read  the interrupt identifier so we know the source.

  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.

  if( id == GIC_SOURCE_TIMER0 ) {
    //PL011_putc( UART0, 'T', true );
    TIMER0->Timer1IntClr = 0x01;
    schedule(ctx); // if the timer caused the interrupt for the right reason we want to schedule the next process because the current process has finished its turn
  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;

  return;
}
