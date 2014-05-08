#include "trunk.h"
#include "inode.h"
#include "AFS_File.h"
#include <string>
#include <iostream>
#include <map>
using namespace std;

void AFS::mkdir(string dirName){
	//error checking dirName
	if (dirName.length()>20) {cout<<"Directory too long. Try again."<<endl;return;}
	
	//check if the dir already exists
	auto &subDirs = pPWD->subDirs;
	if (subDirs.find(dirName)!=subDirs.end()) {cout<<"Directory name already exists. Try a new name"<<endl;return;}
	
	//first create a new iNode
	int inodeLoc = find_next_available_inode();
	
	if (inodeLoc==-1) {cout<<"No available iNode. Try again."<<endl;return;}
	
	//creat an iNode
	Inode inode;
	inode.init(true,this);
	
	AFS_File nFile(&inode,dirName,pPWDInode->number,this);
	nFile.write_file_to_disk();
	
	delete pPWD;
	pPWD = AFS_File(pPWDInode, this);
}
/*
	//create an iNode
	Inode inode;
	inode.number = inodeLoc;
	inode.type = 0x00;
	//assume a dir-type file can be held by a block
	vector<size_t> blockLoc = find_available_block(1);//all blocks in blockLoc will be marked as used.
	inode.blocks = blockLoc.size();
	for(size_t i=0;i<blockLoc.size();i++)
		inode.block[i]=blockLoc[i];

	//write the inode to persistent storage
	inode.write_inode_to_disk(get_inode_pos(inode.number));
	
	//create a new dir-type file and bind it to the inode
	AFS_File subdir(&inode,dirName,pPWDInode->number,this);
	//write file to persistent storage
	subdir.write_file_to_disk();

	//insert the subdir into subDir 
	subDirs.insert(make_pair(dirName,inode.number));
	//update PWD's presence in storage. assuming there is no need to change the respective iNode.
	pPWD->write_file_to_disk();
}
*/
	
	

	
