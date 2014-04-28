#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include "inode.h"

using namespace std;

struct AFS_File {
  //uint8_t fileType;          //all zero represents dir, otherwise regular files
	size_t inodeNum;
    string fileNanme;
    vector<string> subDirs;    //for dir type only
    AFS_File* parent_dir;
    char *usrData;

    string write_file_to_disk(size_t fileStartPos);
    string read_file_from_disk(size_t fileStartPos);
};

