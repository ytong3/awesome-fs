#include <string>
#include <fstream>

class trunck {
private:
    fstream virtualFile;
    double total_size;
    double used_size;

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
    bool unlinke(string name);
    bool stat(string name);
    bool ls();
    bool cat(string name);
    bool cp(string src, string dest);
    bool tree();
    bool import(string srcname, string destname);
    bool export_(string srcname, string destname);
}
    
    
