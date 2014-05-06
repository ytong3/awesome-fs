#pragma once
#include <fstream>
#include <vector>
#include <assert.h>
#include <string.h>
#include <iostream>
#include "trunk.h"
#include "inode.h"
#define DEBG
using namespace std;

struct Inode;
struct AFS_File;

class AFS {
friend class AFS_File;
friend class Inode;
private:
    const double totalDiskSize;
	const size_t blockSize;//in bytes
    const size_t numOfBlocks;
    const size_t numOfInodesBlocks;
    const size_t numOfInodes;

	const size_t inodeStartBlock;//in blocks
    const size_t rootDirInode;//number of the inode for the root directory
    const size_t blockMapStartPos;
    const size_t inodeMapStartPos;
	const size_t dataRegionStart;//in blocks

    //root dir
    const string rootDirPath;
	AFS_File *pPWD;
	Inode *pPWDInode;

	//present opened file related variables
	Inode *pPFInode;
	AFS_File *pPF;//present file
	string fileOperationFlag;

    //bitmap for inode and data region
    std::vector<bool> blockMap;
    std::vector<bool> inodeMap;
    void setBlockMap(size_t pos, bool val);//this and the next operation couples the memory and disk
    void setInodeMap(size_t pos, bool val);
    vector<size_t> find_available_block(size_t fileSize);
    size_t find_next_available_inode();

    void create_virtual_disk(size_t numBytes);


    //utility tools
    //a position (pos) is a 32-bit unsigned number, serving as the absolute offset of the virtual disk.
    void write_disk(size_t startPos, const void* buffer, size_t bufferSize);
    void read_disk(size_t startPos, void* buffer, size_t bufferSize);
    size_t get_block_pos(size_t blockNum);//get start offset in bytes of a block by its block number
    size_t get_inode_pos(size_t inodeNum);//get start offset in bytes of a inode by its inode number


    //bitmap related
    void reserve_block(size_t startNum, int numBlock);
    void free_block(int startNum, int numBlock);
    size_t get_free_blocks(int numBlock);

    void reserve_inode(int inodeNum);
    void free_inode(int inodeNum);
    size_t get_free_inode();

    //internal utility
    bool formatted;
	

	bool format();
    void mkfs();
    size_t open(string fileName, std::string flag);
    string read(size_t fd, size_t size);
    void write(size_t, std::string size);
    void seek(std::string fs, std::string offset);
    void close(size_t fd);
    void mkdir(std::string dirname);
    void rmdir(std::string dirname);
    void cd(std::string dirname);
    void link(std::string src, std::string dest);
    void unlink(std::string name);
    void stat(std::string name);
    void ls();
    void cat(std::string name);
    void cp(std::string src, std::string dest);
    void tree();
    void import(std::string srcname, std::string destname);
    void export_(std::string srcname, std::string destname);

	//utility for argument parsing
	vector<string> parse_args(string args);
public:

	AFS(size_t numBytes);
    static const std::string DiskFileName;
	void process_command(string args);
};
