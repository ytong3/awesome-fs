#include <stdint.h>
#include <stdlib.h>
#include "inode.h"

void Inode::write_inode_to_disk(size_t nodeStartPos){
	FILE* pF = fopen(FS::DiskFileName.c_str(),"r+b");
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
	*this = *buffer;
	free(buffer);
}

void Inode::display(){
	//for debugging
	cout<<"Type: "<<type<<endl
		<<"Size: "<<size<<endl
		<<"Blocks: "<<blocks<<endl
		<<"block[0] "<<block[0]<<endl
		<<"number "<<number<<endl;
}

/*
void Inode::set_create_time(int time){
	this->ctime = time;
}
*/
