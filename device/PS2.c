#include "PS2.h"

/* This file is the unfortunate PS2 device driver that could have been. I tried to make this file
 * the point where the interrupt was taken to. The plan was to check the input from the mouse or
 * the keyboard, depending on the interrupt and give the kernel back useful information that the
 * driver had interpreted and provided. For example if the '3' button was pressed on the keyboard
 * the plan was to check the first input byte (key press) and provide the relevant address to the
 * user program that was going to run.
 * 
 * Since I figured the only way to call the fork and exec system calls was from a user program, I
 * did not see any method of relaying the data from the driver to some form of user method to then
 * execute the program in question.
 * 
 * I therefore opted to execute the user program from within the kernel as I saw no other option but
 * the program address that I supplied from this driver file did not give the intended output
 * 
 * I therefore regretably gave up trying to get a driver to work and therefore used different methods
 * which still work without a problem.
 */

extern void main_P3(); 
extern void main_P4(); 
extern void main_P5();
extern void main_Philosophers();

void* load_addr( uint8_t p ) {
  if     ( 0x04 == p ) {
    return &main_P3;
  }
  else if( 0x05 == p ) {
    return &main_P4;
  }
  else if( 0x06 == p ) {
    return &main_P5;
  }
  else if (0x19 == p ) {
    return &main_Philosophers; 
  }
  return NULL;
}
char* program_name( uint8_t p ) {
  if     ( 0x04 == p ) {
    return "P3";
  }
  else if( 0x05 == p ) {
    return "P4";
  }
  else if( 0x06 == p ) {
    return "P5";
  }
  else if (0x19 == p ) {
    return "Ph"; 
  }
  return "  ";
}

// this mouse interrupt function is still in use and it works out the realtive change in x and y that the mouse has moved
void mouse_interrupt( int *x, int *y, uint8_t byte_0, uint8_t byte_1, uint8_t byte_2 ) {
    int relative_x = byte_1 - ((byte_0 << 4) & 0x100 ); // converting the input into 9-bit signed values
    int relative_y = byte_2 - ((byte_0 << 3) & 0x100 );
    *x += relative_x; *y -= relative_y; // assigning the new x coordinates y is flipped
    return;
}

// doesn't work, the address for the programs had changed once it had been assigned to the program pointer and executed in the kernel
int keyboard_interrupt( uint8_t byte_0, void* program, char *prog) {
    program = load_addr( byte_0 );
    prog = program_name( byte_0 );
    if(0 == strcmp(prog, "  ")) {
        return 0;
    }
    return -1;
}


