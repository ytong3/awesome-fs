#include "trunk.h"

AFS::AFS(int numBytes){
   create_virtual_disk(numBytes);
}

void AFS::create_virtual_disk(int numBytes){
    std::ofstream ofs("virtualDisk.D", std::ios_base::binary|std::ios_base::out); 
    ofs.seekp(((numBytes*8)<<20)-1);
    ofs.write("",1);
    ofs.close();
}

