#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include <string>
#include <map>
using namespace std;

inline size_t echo(size_t val){
	cout<<"File descriptor: "<<val<<endl;
	return val;
}
	

size_t AFS::open(string fileName, string mode){
	if (!(mode=="w"||mode=="r"||mode=="wr"||mode=="rw")){
		cerr<<"Unrecognized file operation flag"<<endl;
		return 0;
	}
	//assuming we can only edit a file at a time
	if (fileOperationFlag!=""){
		cout<<"Another file was already opened. Close it first."<<endl;
		return 0;
	}

	map<string,int>::iterator mit;
	bool fileNameExist = pPWD->file_exists(fileName,mit);
	if ((mode=="r"||mode=="rb")&&!fileNameExist){
		cerr<<"File not exists.\n";
		return 0;
	}
	if (mode=="w"&&fileNameExist){
		cerr<<"File already exisit. Cannot create a new file. Try a different name.\n";
		return 0;
	}
	
	//read from exisiting file
	if (mode=="r"||mode=="rw"||mode=="wr"){
		pPFInode = new Inode(mit->second,this);
		if (pPFInode->type==0x00){
			cerr<<"Cannot open a directory type.\n";
			delete pPFInode;
			return 0;
		}
		pPF = new AFS_File(pPFInode,this);	
		pPF->offset=0;
		fileOperationFlag = mode;
		
		return echo(pPFInode->number);
	}

	//creating a new file and open it
	size_t inodeLoc = find_next_available_inode();
	if (inodeLoc==-1) {cout<<"No available iNode. Disk Full. Try again."<<endl;return echo(-1);}
	
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
	inode.write_inode_to_disk(this);
	
	//insert the subdir into subDir
	pPWD->subDirs.insert(make_pair(fileName,inode.number));
	//update PWD's representation in disk.
	pPWD->write_file_to_disk();

	pPFInode = new Inode(inode);
	pPF = new AFS_File(pPFInode,fileName,pPWDInode->number,this);
	//create an iNode and dir pair
	pPF->offset=0;
	pPF->write_file_to_disk();
	fileOperationFlag = mode;
	return echo(pPFInode->number);
}
