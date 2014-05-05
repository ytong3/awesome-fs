#include "AFS_File.h"
#include <algorithm>
#include <stdio.h>
using namespace std;

//initialize static memeber
const string AFS::DiskFileName="disk.bin";

AFS::AFS(size_t numBytes)
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
    rootDirPath("/"),
	pPWD(NULL),
	pPWDInode(NULL){

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
    format();
	formatted = true;

}

vector<size_t> AFS::find_available_block(size_t fileSize){
    int numBlocksNeeded = fileSize/blockSize;
    numBlocksNeeded += fileSize%blockSize==0?0:1;
    vector<size_t> res;

    for (size_t i=0;i<blockMap.size()&&numBlocksNeeded>0;i++){
        if (blockMap[i]==true) {
            //found a free slot
            setBlockMap(i,false);
            res.push_back(i);
            numBlocksNeeded--;
        }
    }
    return res;
}

size_t AFS::find_next_available_inode(){
    for (size_t i=0;i<inodeMap.size();++i){
        if (inodeMap[i]==true){
            setInodeMap(i,false);
            return i;
        }
    }
    return inodeMap.size();
}

void AFS::setBlockMap(size_t pos, bool val){
   //change blockMap in memory
   //error checking
   assert(pos<numOfBlocks);
   blockMap[pos]=val;

	//updating their disk presentation
   char buffer = 0x00;
   for (int i=(pos/8)*8,j=0;i<(pos/8+1)*8;i++,j++){
   		if (blockMap[i])
			buffer|=1<<j;//set 1;
	}
   //sync the change with block map on disk
   FILE *pF = fopen(DiskFileName.c_str(),"r+b");
   fseek(pF, pos/8, SEEK_SET);
   fwrite(&buffer,1,1,pF);
   fclose(pF);
}

void AFS::setInodeMap(size_t pos, bool val){
    assert(pos<numOfInodes);
    inodeMap[pos] = val;

	char buffer = 0x00;
	for (int i= (pos/8)*8,j=0;i<(pos/8+1)*8;i++,j++){
		if (blockMap[i])
			buffer|=1<<j;//set 1
	}

	FILE *pF = fopen(DiskFileName.c_str(),"r+b");
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

	//set bits before dataReginStart-th bit in both memory and sik
	//for (int i=0;i<dataRegionStart;i++)
	//	bufferMapBlock[i/8]|=1<<(i%8);
	for (int i=0;i<dataRegionStart;i++)
		setBlockMap(i,false);

	//write the buffer to disk
	FILE* pFile = fopen(DiskFileName.c_str(),"r+b");		//must use read/update mode to realize overwriting existing file
	if (!pFile) {perror("Errors occurs while formatting the disk");exit(2);}
	printf("blockMapStartPos = %d\n", blockMapStartPos);
	fseek(pFile,blockMapStartPos,SEEK_SET);
	fwrite(bufferMapBlock,1,numOfBlocks/8+1,pFile);
	free(bufferMapBlock);
	perror("Sync the blockMap to disk");

	//initialize inodeMap in both memory and disk
    inodeMap.resize(numOfInodes,true);
	perror("creating inodeMap in memory");
   
    

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
    rootInode.block[0] = dataRegionStart;
    rootInode.number = 0;
    
    //occupy the first block

    //create the actual file
    AFS_File rootDir(&rootInode, "/", -1,this);
    //write file to disk
    setBlockMap(dataRegionStart,false);
    rootDir.write_file_to_disk();

	//update the respective inode
	rootInode.size = rootDir.dataSize+sizeof(int);
	rootInode.write_inode_to_disk(get_inode_pos(0));
	setInodeMap(0,false);//reserved for the root dir

	//test class AFS_File and inode
	#ifdef DEBG
	//print rootInode info
	printf("Info about rootInode");
	printf("Type=%d, # of blocks=%d, first block #: %d, inode 3: %d\n",rootInode.type,rootInode.blocks,rootInode.block[0],rootInode.number);
	printf("Creating a new inode and read info from disk\n");
	Inode testInode;
	testInode.read_inode_from_disk(get_inode_pos(0));

	rootDir.display_dir_info();
	#endif //DEBG
	//AFS_File testDir(&rootInode,"/",-1,this);
	//testDir.read_file_from_disk();
	
	AFS_File testDir(&rootInode,this);
	testDir.display_dir_info();
	
	cout<<"set pPWDInode"<<endl;
	pPWDInode = new Inode(rootInode);
	cout<<"set pPWD"<<endl;	
	//pPWD = new AFS_File(rootDir);//use default copy constructor.
	//BUG!! pPWD cannot copy from rootDir. rootDir is binded with rootNode, which is a temperorary variable. copying 
	//rootDir to pPWD in effect bind pPWD to rootNode, which not illegal.
	//when creating files in AFS, always follow the same protocol, first create iNode, then the respective file.
	pPWD = new AFS_File(pPWDInode, this);
	cout<<pPWD->fileName;
	return true;
}

void AFS::create_virtual_disk(size_t numBytes){
  cout<<"DiskFileName: "<<DiskFileName<<endl;
	FILE *pFile = fopen(DiskFileName.c_str(),"wb");
	if (!pFile) {perror("Error on file open");exit(1);}
	char *buffer = (char*)malloc(numBytes);

	memset(buffer,0x00,numBytes);

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

void AFS::write_disk(size_t startPos, const void* buffer, size_t bufferSize){
    //check if the disk is open. If not open it with the output mode.
    //Then write to the file from the given start position with the contents in the buffer;
    FILE *pFile;
    pFile = fopen(DiskFileName.c_str(),"r+b");
	if (!pFile) {fprintf(stderr,"File is not opened successfully");exit(3);}
    fseek(pFile,startPos,SEEK_SET);
	fwrite(buffer,1,bufferSize,pFile);
	fclose(pFile);
}

vector<string> AFS::parse_args(string args){	
	int pos=args.length()-1;
	//strip off all trailing spaces
	while (isspace(args[pos])) pos--;

	args.resize(pos+1);
	
	vector<string> res;
	pos = 0;
	while(pos<args.length()){
		//skip all spaces
		while(pos<args.length()&&isspace(args[pos])) pos++;
		int spacePos=args.find(" ",pos);//BUG: should specify the start postion to search.
		if (spacePos==string::npos){
		 	res.push_back(args.substr(pos));	
			break; 
		}
		res.push_back(args.substr(pos,spacePos-pos));
		pos=spacePos+1;
	}
	return res;
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

void AFS::process_command(string argStr){
	vector<string> args = parse_args(argStr);
	if (args.size()==0) {cerr<<"Invalid commands.\n";return;}
	
	if (args[0]=="format") format();
	else if (args[0]=="ls") 
	{
		ls();	
	}
	else if (args[0]=="cd"){
		if (args.size()!=2){
			cerr<<"Too few arguments"<<endl;
			return;
		}
		cd(args[1]);
	}
	else if (args[0]=="mkdir"){
		if (args.size()!=2){
			cerr<<"Too few arguments"<<endl;
			return;
		}
		mkdir(args[1]);
	}
	else{
		cerr<<"Command not found\n";
	}
	cout<<pPWD->fileName;
}
