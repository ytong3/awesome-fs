#include "trunk.h"
#include "AFS_File.h"
#include "inode.h"
#include <cstdlib>
#include <cstring>
#include "assert.h"

using namespace std;

bool AFS::get_inode_by_path(string path, bool newFile, size_t &inode_num){
	if (path.empty()) return false;
	
	//supports only absolute path
	if (path.front()!='/') return false;
	if (path.back()=='/') return false;
	
	vector<string> path_dirs;



	int pos = 1;
	while (pos<path.length()){
		int spos = path.find("/",pos);
		if (spos==string::npos){
			path_dirs.push_back(path.substr(pos));
			break;
		}else{
			path_dirs.push_back(path.substr(pos,spos-pos));
			pos=spos+1;
		}
	}

	cout<<"In cp: path_dirs ";
	for(string &e:path_dirs)
		cout<<e<<" ";
	cout<<endl;

	//from the root dir all the way to the last dir
	size_t parentInodeNum = 0;
	for (string &dir:path_dirs){
		cout<<"current dir/file: "<<dir<<endl;
		Inode curInode(parentInodeNum,this);
		if (dir!=path_dirs.back()&&curInode.type==0x11) return false;//if any intermediate file is not a directory
		cout<<"here"<<endl;
		AFS_File curFile(&curInode,this);
		map<string,int>::iterator mit;
		bool fileExists = curFile.file_exists(dir,mit);

		if (dir!=path_dirs.back()&&!fileExists) return false;
		
		if (dir==path_dirs.back()){
			if (fileExists){
				if (newFile) return false;
				else parentInodeNum = mit->second;
			}else{
				if (!newFile) return false;
				else{
					Inode nInode;
					nInode.init(false,this);
					nInode.write_inode_to_disk(this);
					parentInodeNum = nInode.number;
					AFS_File nFile(&nInode,dir,curInode.number,this);
					nFile.write_file_to_disk();
				}
			}
		}else{
			if (fileExists) parentInodeNum = mit->second;
			else return false;
		}
	}
					
	inode_num = parentInodeNum;
	return true;
}


void AFS::cp(string src, string dest){
	size_t srcInodeNum(0),destInodeNum(0);
	if (!get_inode_by_path(src,false,srcInodeNum)){
		cerr<<"Source file does not exist."<<endl;
		return;
	}
	
	if (get_inode_by_path(dest,false,destInodeNum)){
		cerr<<"Destintation file already exisits."<<endl;
		return;
	}

	//create a destination file;
	get_inode_by_path(dest,true,destInodeNum);

	//begin copy
	Inode srcInode(srcInodeNum,this);
	AFS_File srcFile(&srcInode,this);

	Inode destInode(destInodeNum,this);
	AFS_File destFile(&destInode,this);

	destFile.dataSize = srcFile.dataSize;
	destFile.usrData = (char*)malloc(destFile.dataSize);
	memcpy(destFile.usrData,srcFile.usrData,srcFile.dataSize);
	
	//save destFile to disk;
	destFile.write_file_to_disk();
}
