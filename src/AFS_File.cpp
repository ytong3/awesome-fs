#include "AFS_File.h"
#include <cstring>

AFS_File::AFS_File(uint8_t type, string fileName, int inodeNum, AFS_File* parentDir, AFS* FS){
    this->fileName = fileName;
    fileType = type;//0x00 for dir 0x11 for others
    this->inodeNum = inodeNum;
    this->parentDir = parentDir;
    subDirs.clear();
    this->FS=FS;

    //false-proof: let usrData points to a EOF
    this->dataSize = 1;
    this->usrData = (char*)malloc(1);
    usrData[0]='\0';

    //initialize map
    subDirs.insert(make_pair(this->fileName,this->inodeNum));
}

void AFS_File::write_file_to_disk(size_t fileStartPos){
	if (fileType == 0x00){
		//if file is a dir. write the subDirs to disk
		string buffer;

        auto mit = subDirs.begin();
        for (;mit!=subDirs.end();mit++){
            char inodeNum[10];
            sprintf(inodeNum,"%d",mit->second);
            buffer.append(mit->first+"::"+inodeNum);
			      buffer.append("|");
        }

    assert(buffer.length()<200);
    char bufferStr[200];
    strcpy(bufferStr,buffer.c_str());
		int len = strlen(bufferStr);

		FS->write_disk(fileStartPos,&len,sizeof(int));
		fileStartPos+=sizeof(int);
		FS->write_disk(fileStartPos,bufferStr,len);
	}
	else{
		//if file is a regular file. just write the size and the data to file
		FS->write_disk(fileStartPos,&dataSize,sizeof(int));
		fileStartPos+=sizeof(int);
		FS->write_disk(fileStartPos,&usrData,strlen(usrData));//must include the last EOF
	}
}

void AFS_File::read_file_from_disk(size_t fileStartPos){
    //for a dir type
	if (fileType==0x00){
		FILE *pFile = fopen(AFS::DiskFileName.c_str(),"rb");
		int len;

		fseek(pFile,fileStartPos,SEEK_SET);
		fread(&len,sizeof(int),1,pFile);
		fileStartPos+=sizeof(int);
		char* buffer = (char*) malloc(len);
		fread(&buffer,sizeof(char),len, pFile);
		fclose(pFile);
		buffer[len] = '\0';


		//parse buffer and put the elements in the vector of subDirs
		string bufferStr(buffer);
		int pos = 0;
		while (pos<bufferStr.length()-1){
			int delimPos = bufferStr.find("|");
			string tempPair = bufferStr.substr(pos,delimPos-pos+1);
      int pairDelimPos = tempPair.find("::");
      subDirs.insert(make_pair(tempPair.substr(0,pairDelimPos-1),stoi(tempPair.substr(pairDelimPos+2,delimPos-pairDelimPos-1),nullptr)));
			pos = delimPos+1;
		}

		//clean up memory
		free(buffer);
	}else{
		FILE *pFile = fopen(AFS::DiskFileName.c_str(),"rb");
		if (!pFile) {perror("File Open Failure");exit(3);}
		fread(&dataSize,sizeof(int),1,pFile);
		usrData = (char*)malloc(dataSize+1);
		fread(&usrData,1,dataSize,pFile);
		usrData[dataSize]='\0';
		fclose(pFile);
	}
}
