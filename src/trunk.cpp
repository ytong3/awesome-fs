#include "trunk.h"

AFS::AFS(int numBytes){
   create_virtual_disk(numBytes);
}

void AFS::create_virtual_disk(size_t numBytes){
    std::ofstream ofs("virtualDisk.D", std::ios_base::binary|std::ios_base::out); 
    ofs.seekp(((numBytes*8)<<20)-1);
    ofs.write("",1);
    ofs.close();
}

uint32_t get_block_pos(size_t blockNum){
    return blockNum*blockSize;
}

bool write_disk(size_t startPos, string buffer){
    //check if the disk is open. If not open it with the output mode.
    //Then write to the file from the given start position with the contents in the buffer;
} 

bool read_disk(int startPos, int readSize, string& buffer){
    //check if the disk is open. If not, open it and set to input mode.
    //then read given size of content from from the start pos to the the buffer;

