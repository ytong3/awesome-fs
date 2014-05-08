#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <cstdlib>
#include <cstring>
#include "assert.h"

using namespace std;

void AFS::cat(string fileName){
	//open a file
	map<string,int>::iterator mit;
	if(!pPWD->file_exists(fileName,mit)){
		cerr<<"File not exist!\n";
		return;
	}

	Inode inode(mit->second,this);
	if (inode.type!=0x11){
		cerr<<"Cannot cat a directory\n";
		return;
	}
	AFS_File nFile(&inode,this);

	//size_t dataStartPos = sizeof(int)*2+nFile.fileNameLen;

	string buffer(nFile.usrData,nFile.dataSize);
	buffer.push_back('\0');
	cout<<buffer<<endl<<"=====END OF FILE===="<<endl;
}
