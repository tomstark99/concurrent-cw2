#ifndef __PS2_H
#define __PS2_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

#include "PL050.h"
#include "PL111.h"
#include   "SYS.h"
#include "hilevel.h"

extern void mouse_interrupt( mouse_t* mouse, uint8_t bit_0, uint8_t bit_1, uint8_t bit_2 );
//extern int keyboard_interrupt( uint8_t bit_0, void* program, char* prog);

#endif
