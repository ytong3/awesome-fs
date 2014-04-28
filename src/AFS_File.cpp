#include "AFS_File.h"

AFS_File::AFS_File(uint8_t type, string fileName, int inodeNum, AFS_File* parent_dir){
    this.fileName = fileName;
    fileType = type;//0x00 for dir 0x11 for others
    this.inodeNum = inodeNum;
    this.parent_dir = parent_dir;
    
    //false-proof: let usrData points to a EOF
    this.dataSize = 0;
    this.usrData = (char*)malloc(1);
    usrData[0]='\0';

}

void AFS_File::write_file_to_disk(size_t fileStartPos, AFS& FS){
	if (fileType == 0x00){
		//if file is a dir. write the subDirs to disk
		string buffer;
		for (string& dir:subDirs)
			buffer.append(dir).append("|");
		
		char *bufferStr = buffer.c_str();
		int len = strlen(bufferStr);

		FS.write_disk(fileStartPos,&len,sizeof(int));
		fileStartPos+=sizeof(int);
		FS.write_disk(fileStatrPos,bufferStr,len);
	}
	else{
		//if file is a regular file. just write the size and the data to file
		FS.write_disk(fileStartPos,&dataSize,sizeof(int));
		fileStartPos+=sizeof(int);
		FS.write_disk(fileStartPos,&usrData,strlen(usrData));//must include the last EOF
	}
}

void AFS_File::read_file_from_disk(size_t fileStartPos, AFS& FS){
	if (fileType==0x00){
		FILE *pFile = fopen(AFS::DiskFileName.c_str(),"rb");
		int len;

		fseek(pFile,fileStartPos,SEEK_SET);
		fread(&len,sizeof(int),1,pFile);
		fileStartPos+=sizeof(int);
		char* buffer = (char*) malloc(len);
		fread(&buffer,sizeof(char),len);
		fclose(pFile);
		buffer[len] = '\0'


		//parse buffer and put the elements in the vector of subDirs
		string bufferStr(buffer);
		int pos = 0;
		while (pos<bufferStr.length()-1){
			int delimPos = bufferStr.find("|");
			subDirs.push_back(bufferStr.sub_str(pos,delimPos-pos+1));
			pos = delimPos+1;
		}

		//clean up memory
		free(buffer);
	}else{
		FILE *pFile = fopen(AFS::DiskFileName.c_str(),"rb");
		if (!pFile) {perror("File Open Failure");exit(3);}
		fread(&dataSize,sizeof(int),1,pFile);
		usrData = (char*)malloc(dataSize+1);
		fread(&usrData,1,usrData);
		usrData[dataSize]='\0';
		fclose(pFile);
	}
}
    
    
