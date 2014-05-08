#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include <cstring>
#include "assert.h"

using namespace std;

void AFS::seek(size_t fd, size_t offset){
	if (fileOperationFlag==""){
		cerr<<"No file opened."<<endl;
		return;
	}

	pPF->offset = offset;
}
