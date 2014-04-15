#include <stdint.h>

struct Inode{
    uint8_t type;//all 1 for regular file and all 0 for directory
	uint16_t mode;
	uint16_t size;//size in bytes
	uint32_t ctime;//create time
	uint32_t blocks;//how many blocks
    uint32_t block[10];

	void set_create_time(int time);
};
