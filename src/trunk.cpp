#include "trunk.h"
#include "inode.h"
#include "AFS_File.h"
#include <algorithm>
#include <stdio.h>
using namespace std;

//initialize static memeber
const string AFS::DiskFileName="disk.bin";

AFS::AFS(int numBytes)
    :totalDiskSize(numBytes),
    blockSize(4<<10),
    numOfBlocks(totalDiskSize/blockSize),
    numOfInodesBlocks(10),//set aside 10 blocks for inodes
    numOfInodes(numOfInodesBlocks*(blockSize/sizeof(Inode))),
    inodeStartBlock(3),//the first block is the super block, the second and third are bitmaps
    rootDirInode(0),
    blockMapStartPos(1*blockSize),
    inodeMapStartPos(2*blockSize),
    dataRegionStart(inodeStartBlock+numOfInodesBlocks),
    rootDirPath("/"){

    create_virtual_disk(numBytes);

    cout<<"Initialization succeeded\n";
	cout<<"Virtual Disk size: "<<totalDiskSize/(1<<20)<<"M Bytes\n"
		<<"sizeof inode :"<<sizeof(Inode)<<endl
		<<"inode per block"<<blockSize/sizeof(Inode)<<endl
        <<"Block size: "<<blockSize<<endl
		<<"# of blocks: "<<numOfBlocks<<endl
		<<"maximum number of files (inodes): "<<numOfInodes<<endl
		<<"Data region starts at block "<<dataRegionStart<<endl
		<<"# of Data region blocks: "<<numOfBlocks-dataRegionStart<<endl;
    //format();
	  //formatted = true;
}

vector<int> AFS::find_available_block(size_t fileSize){
    int numBlocksNeeded = fileSize/blockSize;
    numBlocksNeeded += fileSize%blockSize==0?0:1;
    vector<int> res;

    for (int i=0;i<blockMap.size()&&numBlocksNeeded>0;i++){
        if (blockMap[i]==true) {
            //found a free slot
            setBlockMap(i,false);
            res.push_back(i);
            numBlocksNeeded--;
        }
    }
    return res;
}

int AFS::find_next_available_inode(){
    for (int i=0;i<inodeMap.size();i++){
        if (inodeMap[i]==true){
            setInodeMap(i,false);
            return i;
        }
    }
    return -1;
}


void AFS::setBlockMap(size_t pos, bool val){
   //change blockMap in memory
   //error checking
   assert(pos>=numOfBlocks);
   blockMap[pos]=val;

   char buffer = 0x00;
   for (int i=(pos/8)*8,j=0;i<(pos/8+1)*8;i++,j++){
   		if (blockMap[i])
			buffer|=1<<j;//set 1;
	}

   //sync the change with block map on disk
   FILE *pF = fopen(DiskFileName.c_str(),"wb");
   fseek(pF, pos/8, SEEK_SET);
   fwrite(&buffer,1,1,pF);
   fclose(pF);
}

void AFS::setInodeMap(size_t pos, bool val){
    assert(pos>=numOfInodes);
    inodeMap[pos] = val;

	char buffer = 0x00;
	for (int i= (pos/8)*8,j=0;i<(pos/8+1)*8;i++,j++){
		if (blockMap[i])
			buffer|=1<<j;//set 1
	}

	FILE *pF = fopen(DiskFileName.c_str(),"wb");
	fseek(pF,pos/8,SEEK_SET);
	fwrite(&buffer,1,1,pF);
	fclose(pF);
}

size_t AFS::get_block_pos(size_t blockNum){
    return blockSize*blockNum;
}

size_t AFS::get_inode_pos(size_t inodeNum){
	int inodesPerBlock = blockSize/sizeof(Inode);
	//determine which block the node should be in
	int inodeBlockNum = inodeNum/inodesPerBlock;
	int offset = inodeNum%inodesPerBlock;
	return (inodeStartBlock+inodeBlockNum)*blockSize+offset*sizeof(Inode);
}

bool AFS::format(){
	using FileType=AFS_File;

    //display warning msg

    //initialize blcok maps in memory
    blockMap.resize(numOfBlocks,true);
    fill_n(blockMap.begin(),dataRegionStart,false);
	perror("Creating blockMap in memory");

	//assume block map occupy less than a block
	assert(numOfBlocks/8+1<blockSize);
	char *bufferMapBlock = (char*) malloc(numOfBlocks/8+1);
	memset(bufferMapBlock,0x00,numOfBlocks/8+1);

	//set bits before dataReginStart-th bit
	for (int i=0;i<dataRegionStart;i++)
		bufferMapBlock[i/8]|=1<<(i%8);

	//write the buffer to disk
	FILE* pFile = fopen(DiskFileName.c_str(),"r+b");		//must use read/update mode to realize overwriting existing file
	rewind(pFile);
	if (!pFile) {perror("Errors occurs while formatting the disk");exit(2);}
	printf("blockMapStartPos = %d\n", blockMapStartPos);
	fseek(pFile,blockMapStartPos,SEEK_SET);
	fwrite(bufferMapBlock,1,numOfBlocks/8+1,pFile);
	free(bufferMapBlock);
	perror("Sync the blockMap to disk");

	//initialize inodeMap in both memory and disk
    inodeMap.resize(numOfInodes,true);
	perror("creating inodeMap in memory");
    inodeMap[0]=false;//reserved for the root dir

	//assume that map occupy less than a block
	assert(numOfInodes/8+1<blockSize);
	fseek(pFile,inodeMapStartPos,SEEK_SET);
	char *bufferMapInode = (char*)malloc(numOfInodes/8+1);
	memset(bufferMapInode,0x00,numOfInodes/8+1);
	memset(bufferMapInode,0x01,1);
	fwrite(bufferMapInode,1,numOfInodes/8+1,pFile);
	perror("inodeMap written to disk");
	free(bufferMapInode);
	fclose(pFile);

    //create root directory and the corresponding inode
    //first inode
    Inode rootInode;
    rootInode.type = 0x00;
    rootInode.blocks = 1;
    rootInode.block[0] = 0;
    rootInode.number = 0;
    rootInode.write_inode_to_disk(get_inode_pos(0));

    //create the actual file
    AFS_File rootDir(0x00, "/", 0,nullptr,this);
    rootDir.inodeNum = 0;
    //wreite file to disk
    rootDir.write_file_to_disk(get_block_pos(dataRegionStart));

	return true;
}

void AFS::create_virtual_disk(size_t numBytes){
  cout<<"DiskFileName: "<<DiskFileName<<endl;
	FILE *pFile = fopen(DiskFileName.c_str(),"wb");
	if (!pFile) {perror("Error on file open");exit(1);}
	char *buffer = (char*)malloc(numBytes);

	//memset(buffer,0x00,numBytes);

  printf("buffer created and initialized\n");

	//write numBytes of memory into a file may not be secure enough.
	//fix: use a while loop to write 0x00 100<<20 times.
	if (!fwrite(buffer,sizeof(char),numBytes,pFile)||fclose(pFile)==EOF){
		perror("File I/O failure");
		exit(3);
	}
	free(buffer);
	perror("Virtual Disk Created");
}

void AFS::write_disk(size_t startPos, void* buffer, size_t bufferSize){
    //check if the disk is open. If not open it with the output mode.
    //Then write to the file from the given start position with the contents in the buffer;
    FILE *pFile;
    pFile = fopen(DiskFileName.c_str(),"r+b");
	if (!pFile) {fprintf(stderr,"File is not opened successfully");exit(3);}
    fseek(pFile,startPos,SEEK_SET);
	fwrite(buffer,1,bufferSize,pFile);
	fclose(pFile);
}

void AFS::read_disk(size_t startPos, void* buffer, size_t bufferSize){
    //check if the disk is open. If not, open it and set to input mode.
    //then read given size of content from from the start pos to the the buffer;
	FILE *pFile;
	pFile = fopen(DiskFileName.c_str(),"rb");
	if (pFile==NULL) {fprintf(stderr,"File is not opened successfully");exit(3);}
	fseek(pFile,startPos,SEEK_SET);
	fread(buffer,1,bufferSize,pFile);
	fclose(pFile);
}
