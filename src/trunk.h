#include <string>
#include <fstream>
#include "inode.h"
#include <vector>

class AFS {
private:
    fstream virtualDisk;
    const string virtualDiskFileName = "virtualDisk.D";
    const double totalDiskSize = 100*1<<6*sizeof(char);
    double used_size;
	int blockSize = 4<<10;//in bytes
    const size_t numOfBlocks;
    const size_t numOfInodes;
    const size_t numOfInodesBlocks;
	const size_t dataRegionStart;//in blocks
	const size_t inodeStartBlcok;//in blocks
    const size_t rootDirInode;//number of the inode for the root directory
    const size_t blockMapStartPos;
    const size_t inodeMapStartPos;

    //bitmap for inode and data region
    std::vector<bool> blockMap;
    std::vector<bool> inodeMap;
    bool setBlockMap(size_t pos, bool val);//this and the next operation couples the memory and disk
    bool setInodeMap(size_t pos, bool val);


    void create_virtual_disk(int numBytes);
    AFS(int numBytes);

    //utility tools
    //a position (pos) is a 32-bit unsigned number, serving as the absolute offset of the virtual disk.
    bool write_disk(size_t startPos, string& buffer);
    bool read_disk(size_t startPos, size_t readSize, string& buffer);
    size_t get_block_pos(size_t blockNum);//get start offset of a block by its block number
    size_t get_inode_pos(size_t inodeNum);//get start offset of a inode by its inode number
    

    //bitmap related
    void reserve_block(int startNum, int numBlock);
    void free_block(int startNum, int numBlock);
    size_t get_free_blocks(int numBlock);

    void reserve_inode(int inodeNum);
    void free_inode(int inodeNum);
    size_t get_free_inode();

    //internal utility
    bool format();
    
    
    
public:
    bool mkfs();
    bool open(string fileName, string flag);
    bool read(string fd, string size);
    bool write(string fd, string size);
    bool seek(string fs, string offset);
    bool close(string fd);
    bool mkdir(string dirname);
    bool rmdir(string dirname);
    bool cd(string dirname);
    bool link(string src, string dest);
    bool unlink(string name);
    bool stat(string name);
    bool ls();
    bool cat(string name);
    bool cp(string src, string dest);
    bool tree();
    bool import(string srcname, string destname);
    bool export_(string srcname, string destname);
}
    
    
