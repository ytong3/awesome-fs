#include "trunk.h"
#include "AFS_File.h"
#include "inode.h"
#include <map>
#include <string>
using namespace std;

void AFS::rmdir(string dirName){
	//error checking dirName
	if (dirName==".."||dirName=="."){
		cerr<<"Cannot remove"<<dirName<<endl;
		return;
	}
	//check if dirName exists
	auto &subDirs = pPWD->subDirs;
	auto mit = subDirs.find(dirName);
	if (mit==subDirs.end()){
		cerr<<dirName<<" does not exist. Cannot remove it\n";
		return;
	}
	
	//get meta data of the dir to be removed
	Inode toBeRemoved;
	toBeRemoved.read_inode_from_disk(get_inode_pos(mit->second));
	
	//deallocate the respective inode
	setInodeMap(mit->second,true);

	//deallocate the block
	cout<<"In rmdir: block[0]="<<toBeRemoved.block[0]<<endl;
	assert(toBeRemoved.blocks==1);
	setBlockMap(toBeRemoved.block[0],true);

	//remove it from map
	subDirs.erase(mit);
	//updating PWD
	pPWD->write_file_to_disk();
	//assume inode can be left intact.
}
