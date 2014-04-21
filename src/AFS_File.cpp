#include "AFS_File.h"

AFS_File::AFS_File(uint8_t type, string fileName, Inode& inode, AFS_File& parent_dir){
    this.fileName = fileName;
    fileType = type;
    this.numOfInode = inode.number;
    
}
    
    
