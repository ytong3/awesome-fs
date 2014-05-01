#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include "inode.h"

using namespace std;

struct AFS_File {
  uint8_t fileType;          //all zero represents dir, otherwise regular files
	size_t inodeNum;
  string fileName;
  map<string,int> subDirs;    //for dir type only
  AFS_File* parentDir;
  int dataSize;
  char *usrData;
  AFS* FS;                    //file systme that the file lives in

  AFS_File(uint8_t type, string fileName, int inodeNum, AFS_File* parentDir, AFS* FS);
  void write_file_to_disk(size_t fileStartPos);
  void read_file_from_disk(size_t fileStartPos);
};
