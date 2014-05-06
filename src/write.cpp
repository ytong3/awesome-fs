#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include <cstring>
#include "assert.h"

using namespace std;

void AFS::write(size_t fd, string str){
	string &mode = fileOperationFlag;
	if (mode==""){
		cerr<<"requested file was not opened. Open it first.\n";
		return;
	}
	if (fd!=pPFInode->number){
		cerr<<"Another file was already opened. Close it first.\n";
		return;
	}
	if (mode!="w"&&mode!="rw"){
		cout<<"Cannot write file under file operation mode "<<mode<<endl;
		return;
	}

	//file assoc.ed with fd is opened.
	if (pPF->dataSize==0){
	//make use of the first block
		pPF->dataSize=blockSize-sizeof(int)*2-30;
		pPF->usrData = (char*)malloc(pPF->dataSize);
		pPF->dataSize=0;
	}
	
	memcpy(pPF->usrData+pPF->offset,str.c_str(),str.length());
	pPF->offset+=str.length();
	pPF->dataSize = pPF->dataSize>pPF->offset?pPF->dataSize:pPF->offset;
	
	//flush changes to disk
	pPF->write_file_to_disk();
	
}
	
	
