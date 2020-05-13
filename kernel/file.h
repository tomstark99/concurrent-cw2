#ifndef __FILE_H
#define __FILE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include "disk.h"

#define BLOCK_COUNT 64 // 64 4kb blocks
#define BLOCK_LENGTH 4096 // 4kb blocks
#define BITMAP_LENGTH 2048 // 4kb blocks
#define INODE_LENGTH 256 // 256 bytes per inode
#define INODE_PER_BLOCK 16 // 16 inodes per block
#define INODE_BLOCKS 5 // 5 inode blocks 80 inodes in total
#define DATAR_BLOCKS 56 // 56 data region blocks

typedef enum { // the types of files
    DIR, // a directory
    FILE // a file
} file_t;

typedef enum {
    READ,
    WRITE,
    APPEND
} perm_t;

typedef struct {
    file_t type;
    uint32_t pointers[12];
} inode_t;

#endif
