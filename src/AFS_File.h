#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include "inode.h"
#include "trunk.h"

using namespace std;

struct AFS_File {
	Inode* inode;
 	uint8_t fileType;          //all zero represents dir, otherwise regular files
	size_t inodeNum;
  	string fileName;
  	int fileNameLen;
  	map<string,int> subDirs;    //for dir type only
  	int parentInode;
  	int dataSize;
  	char *usrData;
  	AFS* FS;                    //file systme that the file lives in

	size_t offset = 0;
  
  
  //AFS_File() = default;	  //no default constructor is provide, 
  							  //AFS_File object must bound to a inode.

  //AFS_File(Inode* inode,string fileName,AFS* FS);
  AFS_File(Inode* inode, string fileName, int parentInode, AFS* FS);
  AFS_File(Inode* inode,AFS* FS);
  ~AFS_File();
  void write_file_to_disk();
  void serialize_subdir_map();
  void read_file_from_disk();
  void display_dir_info();
  void display_file_info();
  bool file_exists(string name,map<string,int>::iterator&);
};
