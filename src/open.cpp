#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include <string>
#include <map>
using namespace std;

size_t AFS::open(string fileName, string mode){
	//assuming we can only edit a file at a time
	if (fileOperationFlag!=""){
		cout<<"Another file was already opened. Close it first."<<endl;
		return 0;
	}

	map<string,int>::iterator mit;
	bool fileNameExist = pPWD->file_exists(fileName,mit);
	if ((mode=="r"||mode=="rb")&&!fileNameExist){
		cerr<<"File not exists.\n";
		return 0;;
	}
	if (mode=="w"&&fileNameExist){
		cerr<<"File already exisit. Cannot create a new file. Try a different name.\n";
		return 0;
	}
	
	//read from exisiting file
	if (mode=="r"||mode=="rw"||mode=="wr"){
		pPFInode = new Inode(mit->second,this);
		pPF = new AFS_File(pPFInode,this);	
		pPF->offset=0;
		fileOperationFlag = mode;
		return pPFInode->number;
	}

	//creating a new file and open it
	size_t inodeLoc = find_next_available_inode();
	if (inodeLoc==-1) {cout<<"No available iNode. Disk Full. Try again."<<endl;return -1;}
	
	//create an iNode
	Inode inode;
	inode.number = inodeLoc;
	inode.type = 0x11;

	//find a new block to hold the new file
	vector<size_t> blockLoc = find_available_block(1);
	inode.blocks = blockLoc.size();
	for (size_t i=0;i<blockLoc.size();i++)
		inode.block[i]=blockLoc[i];
	
	//write the inode to persistent storage
	inode.write_inode_to_disk(get_inode_pos(inode.number));
	
	//create a new file and bind it to the inode
	AFS_File nFile(&inode,fileName,pPWDInode->number,this);
	nFile.write_file_to_disk();
	
	//insert the subdir into subDir
	pPWD->subDirs.insert(make_pair(fileName,inode.number));
	//update PWD's representation in disk.
	pPWD->write_file_to_disk();

	pPFInode = new Inode(inode);
	pPF = new AFS_File(pPFInode,fileName,pPWDInode->number,this);
	pPF->offset=0;

	fileOperationFlag = mode;
	return pPFInode->number;
}
