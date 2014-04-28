#pragma once
#include <fstream>
#include <vector>
#include <assert.h>
#include <string.h>
#include <iostream>
using namespace std;

class AFS {
private:
    const double totalDiskSize;
	int blockSize;//in bytes
    const size_t numOfBlocks;
    const size_t numOfInodesBlocks;
    const size_t numOfInodes;
	
	const size_t inodeStartBlock;//in blocks
    const size_t rootDirInode;//number of the inode for the root directory
    const size_t blockMapStartPos;
    const size_t inodeMapStartPos;
	const size_t dataRegionStart;//in blocks

    //root dir
    const string rootDirPath = "/";

    //bitmap for inode and data region
    std::vector<bool> blockMap;
    std::vector<bool> inodeMap;
    void setBlockMap(size_t pos, bool val);//this and the next operation couples the memory and disk
    void setInodeMap(size_t pos, bool val);
	
	


    void create_virtual_disk(size_t numBytes);
    

    //utility tools
    //a position (pos) is a 32-bit unsigned number, serving as the absolute offset of the virtual disk.
    void write_disk(size_t startPos, void* buffer, size_t bufferSize);
    void read_disk(size_t startPos, void* buffer, size_t bufferSize);
    size_t get_block_pos(size_t blockNum);//get start offset in bytes of a block by its block number
    size_t get_inode_pos(size_t inodeNum);//get start offset in bytes of a inode by its inode number
    

    //bitmap related
    void reserve_block(int startNum, int numBlock);
    void free_block(int startNum, int numBlock);
    size_t get_free_blocks(int numBlock);

    void reserve_inode(int inodeNum);
    void free_inode(int inodeNum);
    size_t get_free_inode();

    //internal utility
    bool formatted;
	
    
    
    
public:

	AFS(int numBytes);
    static const std::string DiskFileName;
	bool format();
    bool mkfs();
    bool open(std::string fileName, std::string flag);
    bool read(std::string fd, std::string size);
    bool write(std::string fd, std::string size);
    bool seek(std::string fs, std::string offset);
    bool close(std::string fd);
    bool mkdir(std::string dirname);
    bool rmdir(std::string dirname);
    bool cd(std::string dirname);
    bool link(std::string src, std::string dest);
    bool unlink(std::string name);
    bool stat(std::string name);
    bool ls();
    bool cat(std::string name);
    bool cp(std::string src, std::string dest);
    bool tree();
    bool import(std::string srcname, std::string destname);
    bool export_(std::string srcname, std::string destname);
};
    

