#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "trunk.h"

class AFS;

struct Inode{
	typedef AFS FS;
    uint8_t type;//all 1 for regular file and all 0 for directory
	//uint16_t mode;
	uint16_t size;//size of file in bytes
	//uint32_t ctime;//create time
	uint32_t blocks;//how many blocks
    uint32_t block[10];
    size_t number;//inode number
	void display();

	Inode() = default;
	Inode(size_t inodeNum, AFS*);
	void init(bool isDir,AFS*);

	//void set_create_time(int time);
	void write_inode_to_disk(size_t nodeStartPos);
	void write_inode_to_disk(FS* fs);
	void read_inode_from_disk(size_t nodeStartPos);
	void read_inode_from_disk(FS* fs);
};
