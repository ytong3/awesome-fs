#include <iostream>
#include "AFS_File.h"
#include "inode.h"
#include "trunk.h"
#include <string>

using namespace std;

void AFS::stat(string name){
	map<string,int>::iterator mit;
	if (!pPWD->file_exists(name,mit)){
		cerr<<name<<" is not a directory or a file.\n";
		return;
	}
	
	cout<<"stats about "<<name<<endl;
	
	Inode tmpNode(mit->second,this);
	tmpNode.display();
	
	AFS_File tmpFile(&tmpNode, this);
	tmpFile.display_dir_info();
}
	
	
