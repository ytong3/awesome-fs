#include <string>
#include <fstream>
#include "inode.h"

class AFS {
private:
    fstream virtualDisk;
    const double total_size = 100*1<<6*sizeof(char);
    double used_size;
	int blockSize = 4<<10;//in bytes
	int dataRegionStart;//in blocks
	int inodeDataStart;//in blocks
    int rootdirNode;//in blocks

    void create_virtual_disk(int numBytes);
    AFS(int numBytes);

    //utility tools
    //a position (pos) is a 32-bit unsigned number, serving as the absolute offset of the virtual disk.
    void write_disk(size_t startPos, string buffer);
    void read_disk(size_t startPos, size_t readSize, string& buffer);
    size_t get_block_pos(int blockNum); 
    

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
    
    
