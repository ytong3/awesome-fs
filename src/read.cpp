#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include "assert.h"

using namespace std;

string AFS::read(size_t fd, size_t size){	
	string &mode = fileOperationFlag;
	if (mode==""){
		cerr<<"requested file was not opened. Open it first.\n";
		return "ERROR\n";
	}

	if (fd!=pPFInode->number){
		cerr<<"Another file was already opened. Close it first.\n";
		return "ERROR\n";
	}
	if (mode!="r"&&mode!="rb"){
		cout<<"Cannot read file under file operation mode "<<mode<<endl;
		return "ERROR\n";
	}
	
	//assume file is less than a blockSize big.
	assert(pPF->offset+size<blockSize);
	string buffer;
	buffer.assign(pPF->usrData+pPF->offset, size);
	pPF->offset+=size;
	cout<<buffer<<endl;
	return buffer;
}
	
		
	
	
		
	

	
		
	
	
		
	
