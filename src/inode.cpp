#include <stdint.h>
#include <stdlib.h>
#include "inode.h"

void Inode::write_inode_to_disk(size_t nodeStartPos){
	FILE* pF = fopen(FS::DiskFileName.c_str(),"wb");
	fseek(pF,nodeStartPos,SEEK_SET);
	fwrite(this,sizeof(Inode),1,pF);
	fclose(pF);
}

void Inode::read_inode_from_disk(size_t nodeStartPos){
	FILE* pF = fopen(FS::DiskFileName.c_str(),"rb");
	fseek(pF,nodeStartPos,SEEK_SET);
	Inode* buffer = (Inode*) malloc(sizeof(Inode));
	fread(buffer,sizeof(Inode),1,pF);
	fclose(pF);
}

void Inode::set_create_time(int time){
	this->ctime = time;
}
