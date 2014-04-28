#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "trunk.h"

struct Inode{
	typedef AFS FS;
    uint8_t type;//all 1 for regular file and all 0 for directory
	uint16_t mode;
	uint16_t size;//size of file in bytes
	uint32_t ctime;//create time
	uint32_t blocks;//how many blocks
    uint32_t block[10];
    size_t number;

	void set_create_time(int time);
	void write_inode_to_disk(size_t nodeStartPos);
	Inode* read_inode_from_disk(size_t nodeStartPos);
};
