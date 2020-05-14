#include "PS2.h"

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
    return &main_Philosophers; // I made sure to add another program the user could execute from the console calling it 'philosophers'
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
    return "Ph"; // I made sure to add another program the user could execute from the console calling it 'philosophers'
  }
  return "  ";
}

void mouse_interrupt( mouse_t *mouse, uint8_t bit_0, uint8_t bit_1, uint8_t bit_2 ) {

      int relative_x = bit_1 - ((bit_0 << 4) & 0x100 );
      int relative_y = bit_2 - ((bit_0 << 3) & 0x100 );
      mouse->x += relative_x; mouse->y -= relative_y;

      if(mouse->y >= mouse->y_max) mouse->y = mouse->y_max; // check the mouse position has not exceeded the boundaries of the screen
      else if(mouse->y <= mouse->y_min) mouse->y = mouse->y_min;
      if(mouse->x >= mouse->x_max) mouse->x = mouse->x_max;
      else if(mouse->x <= mouse->x_min) mouse->x = mouse->x_min;
    return;
}

int keyboard_interrupt( uint8_t bit_0, void* program, char *prog) {
    program = load_addr( bit_0 );
    prog = program_name( bit_0 );
    if(0 == strcmp(prog, "  ")) {
        return 0;
    }
    return -1;
}


