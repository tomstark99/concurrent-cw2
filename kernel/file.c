#include "file.h"

void read_inode_block(int inode_num, inode_t* i) {
    unit8_t block[BLOCK_LENGTH] = {0};
    disk_rd(inode_num + BITMAP_LENGTH, block);
    memcpy(i, block, sizeof(inode_t));
}

void write_inode_block(int inode_num, inode_t* i) {
    unit8_t block[BLOCK_LENGTH] = {0};
    memcpy(block, i, sizeof(inode_t));
    disk_wr(inode_num + BITMAP_LENGTH, block);
}

void write_data_block(int data_block_num, uint8_t* b) {
    disk_wr(data_block_num + BITMAP_LENGTH + INODE_LENGTH, b);
}

void read_data_block(int data_block_num, uint8_t* b) {
    disk_rd(data_block_num + BITMAP_LENGTH + INODE_LENGTH, b);
}
