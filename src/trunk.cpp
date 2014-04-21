#include "trunk.h"
#include <iostream>
#include <algorithm>
AFS::AFS(int numBytes)
    :totalDiskSize(100<<20),
    used_size(0),
    blockSize(4<<10),
    numOfBlocks(totalDiskSize/blockSize),
    numOfInodes(double(numOfBlocks-3)/(1+sizeof(Inode)/double(blockSize))),
    numOfInodesBlocks(numOfInodes/(blockSize/sizeof(Inode))+numOfInodes%(blockSize/sizeof(Inode))>0?1:0),
    inodeStartBlcok(3),
    dataRegionStart(inodeStartBlcok+numOfInodesBlocks);
    rootDirInode(0),
    blockMapStartPos(1*blockSize*8),
    inodeMapStartPos(2*blockSize*8)
    {
        create_virtual_disk(numBytes);
        format();
        
        //display summary of Initialization
        std::cout<<"Initialization succeeded\n";
        std::cout<<"Virtual Disk size: "<<totalDiskSize<<"M Bytes\n"
                 <<"Block size: "<<blockSize<<std::endl
                 <<"# of blocks: "<<numOfBlocks<<std::endl
                 <<"maximum number of files (inodes): "<<numOfInodes<<std::endl
                 <<"Data region blocks: "<<numOfBlocks-dataRegionStart<<std::endl;
    }

bool setBlockMap(size_t pos, bool val){
   //change blockMap in memory
   //error checking
   if (pos>=numOfBlock) return false;
   blockMap[pos]=val;
   string strVal('0'+val?1:0);
   return write_disk(blockMapStartPos+pos,strVal);
}

bool setInodeMap(size_t pos, bool val){
    if (pos>=numOfInodes) return false;
    inodeMap[pos] = val;
    string strVal('0'+val?1:0);
    return write_disk(inodeMapStartPos+pos, strVal);
}

size_t get_block_pos(size_t blockNum){
    return blockSize*8*blockNum;
}

size_t get_inode_pos(size_t inodeNum){
    return inodeStartBlcok*blockSize*8+inodeNum*sizeof(Inode)*8;
}

bool AFS::format(){
    //display warning msg

    //initialize maps
    blockMap.resize(numOfBlocks,true);
    std::fill_n(blockMap.begin(),dataRegionStart,false);

    string blockStr;

    for(int i=0;i<numOfBlocks;i++){
        blockMapStr.append(1,'0'+blockMap[i]?1:0);
    }
    write_disk(blockMapStartPos, blockMapStr);

        
    inodeMap.resize(numOfInodes,true);
    inodeMap[0]=false;//reserved for the root dir
    string inodeMapStr;
    for(int i=0;i<numOfInodes;i++)
        inodeMapStr.append(1,'0'+inodeMap[i]?1:0);

    write_disk(inodeMapStartPos,inodeMapStr);

    //create root directory and the corresponding inode
    

}
    
void AFS::create_virtual_disk(size_t numBytes)

    {
        std::ofstream ofs("virtualDisk.D", std::ios_base::binary|std::ios_base::out); 
        ofs.seekp(((numBytes*8)<<20)-1);
        ofs.write("",1);
        ofs.close();
    }

uint32_t get_block_pos(size_t blockNum){
    return blockNum*blockSize*8;        //all pos must be in terms of bit
}

bool write_disk(size_t startPos, string buffer){
    //check if the disk is open. If not open it with the output mode.
    //Then write to the file from the given start position with the contents in the buffer;
} 

bool read_disk(int startPos, int readSize, string& buffer){
    //check if the disk is open. If not, open it and set to input mode.
    //then read given size of content from from the start pos to the the buffer;

