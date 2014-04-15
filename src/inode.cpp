#include <stdint.h>

struct Inode{
	uint16_t mode;
	uint16_t size;//size in bytes
	uint32_t ctime;//create time
	uint32_t blocks;//how many blocks

	void set_create_time(int time);
};
