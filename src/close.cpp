#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include "assert.h"

using namespace std;

void AFS::close(size_t fd){
	string &mode = fileOperationFlag;
	if (mode==""){
		cerr<<"No file opened\n";
		return;
	}
	
	if (fd!=pPFInode->number){
		cerr<<"File assosicated with FD "<<fd<<" not opened.\n";
		return;
	}

	//file assoc. with fd is opened
	pPF->write_file_to_disk();//save changes to disk;
	
	//destroy respective inode and file in memory
	delete pPFInode;
	pPFInode=NULL;
	delete pPF;
	pPF=NULL;
	//set mode to empty 
	fileOperationFlag="";
	return;
}
	

