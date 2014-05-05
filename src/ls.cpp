#include "trunk.h"
#include "AFS_File.h"

void AFS::ls(){
	if (pPWD) 
		pPWD->display_dir_info();
}
