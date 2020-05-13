
#include "lcd.h"

extern void main_P3(); 
extern void main_P4(); 
extern void main_P5();
extern void main_Philosophers();

int num_active = 1;

void* load_addr( uint8_t p ) {
  if     ( 4 == p ) {
    return &main_P3;
  }
  else if( 5 == p ) {
    return &main_P4;
  }
  else if( 6 == p ) {
    return &main_P5;
  }
  else if (25 == p ) {
    return &main_Philosophers; // I made sure to add another program the user could execute from the console calling it 'philosophers'
  }
  return NULL;
}

void main_lcd() {
  while( 1 ) {
    char s[10];
    uint8_t press = PL050_getc( PS20 );

    uint8_t click = PL050_getc( PS21 );
    itoa(s, click);
    write(STDOUT_FILENO, s, strlen(s));

    if( press == 4 || press == 5 || press == 6 || press == 25 ) {
      void* addr = load_addr( press );
      if( addr != NULL ) {
        if( 0 == fork() ) {
          draw( press );
          exec( addr );
        }
        else {
        }
      }
      else {
        //puts( "unknown program\n", 16 );
      }
    } 
    else if( press == 20 ) {
      kill_all( SIG_TERM );
    } 
    // else {
    // }
  }
  exit( EXIT_SUCCESS );
}
