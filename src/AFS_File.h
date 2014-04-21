#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

struct AFS_File {
    uint8_t fileType;          //all zero represents dir, otherwise regular files
    string fileNanme;
    vector<string> subDirs;    //for dir type only
    int numOfInode;

    string to_string();
}

