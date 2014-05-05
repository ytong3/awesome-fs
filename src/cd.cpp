#include "trunk.h"
#include "AFS_File.h"
#include <map>
#include <string>
using std::map;
using std::string;

void AFS::cd(string dirName){
	map<string,int>	&subdirs = pPWD->subDirs;
	auto mit = subdirs.find(dirName);
	if (mit==subdirs.end()){
		cerr<<"Directory "<<dirName<<" not found."<<endl;
		return;
	}

	if (mit->first==".") return;
	if (mit->first==".."){
		//change to parent dir
		delete pPWD;
		delete pPWDInode;
		pPWDInode = new Inode;
		pPWDInode->read_inode_from_disk(get_inode_pos(mit->second));
		pPWD = new AFS_File(pPWDInode, this);
		return;
	}

	//if dir is found
	//delete pPWD in the dynamic memory
	delete pPWDInode;
	delete pPWD;
	cout<<"pPWD and pPWDInode reset"<<endl;
	pPWDInode = new Inode;
	pPWDInode->read_inode_from_disk(get_inode_pos(mit->second));
	cout<<"reloading inode completed"<<endl;
	pPWDInode->display();
	
	pPWD = new AFS_File(pPWDInode, this);
	cout<<"dir loaded"<<endl;
	#ifdef DEBG
		cout<<"Current Dir: "<<pPWD->fileName<<endl;
		pPWD->display_dir_info();
	#endif
}
	
	
	
	
