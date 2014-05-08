#include <stdint.h>
#include <stdlib.h>
#include <algorithm>
#include "inode.h"
using namespace std;

//for reading from disk only. Do not use it for creating a new inode.
Inode::Inode(size_t inodeNum, AFS* pAFS){
	read_inode_from_disk(pAFS->get_inode_pos(inodeNum));
}

void Inode::init(bool isDir,AFS* pAFS){
	number = pAFS->find_next_available_inode();
	type = isDir?0x00:0x11;
	blocks = 0;
	auto blockAllocation = find_available_block(1);
	copy(blockAllocation.begin(),blockAllocation.end(),block);
}

void Inode::write_inode_to_disk(size_t nodeStartPos){
	FILE* pF = fopen(FS::DiskFileName.c_str(),"r+b");
	fseek(pF,nodeStartPos,SEEK_SET);
	fwrite(this,sizeof(Inode),1,pF);
	fclose(pF);
}

void Inode::write_inode_to_disk(FS* fs){
	size_t nodeStartPos = fs->get_inode_pos(number);
	write_inode_to_disk(nodeStartPos);
}

void Inode::read_inode_from_disk(size_t nodeStartPos){
	FILE* pF = fopen(FS::DiskFileName.c_str(),"rb");
	fseek(pF,nodeStartPos,SEEK_SET);
	Inode* buffer = (Inode*) malloc(sizeof(Inode));
	fread(buffer,sizeof(Inode),1,pF);
	fclose(pF);
	*this = *buffer;
	free(buffer);
}

void Inode::read_inode_from_disk(FS* fs){
	size_t nodeStartPos = fs->get_inode_pos(number);
	read_inode_from_disk(nodeStartPos);
}

void Inode::display(){
	//for debugging
	cout<<"Type: "<<string(type==0x00?"Directory":"Regular File")<<endl
		<<"Size: "<<size<<endl
		<<"Number of Blocks: "<<blocks<<endl
		<<"First block: "<<block[0]<<endl
		<<"iNode number: "<<number<<endl;
}

/*
void Inode::set_create_time(int time){
	this->ctime = time;
}
*/
