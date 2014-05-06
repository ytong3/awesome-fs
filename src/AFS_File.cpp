#include "AFS_File.h"
#include <cstring>
//#define DEBG

//used when creating new subdir under parent dir.
AFS_File::AFS_File(Inode* inode, string fileName, int parentInode, AFS* FS){
	//this constructor only creates and binds this file object to a inode.
	//other methods needs to be used to load or write contents of the file.
    this->fileName = fileName;
	this->fileNameLen = fileName.length();
    fileType = inode->type;//0x00 for dir 0x11 for others
    this->parentInode = parentInode;
    subDirs.clear();
    this->FS=FS;
	this->offset = 0;

	//binding inode
	this->inode = inode;
	this->inodeNum = inode->number;

    //false-proof: let usrData points to a EOF
    dataSize = 0;
    usrData = NULL;

	if (fileType == 0x00){
		subDirs.insert(make_pair(".",this->inodeNum));
		if(parentInode!=-1) subDirs.insert(make_pair("..", parentInode));
		serialize_subdir_map();
	}
}



//use this ctor if file can be read from disk, and specifically 
AFS_File::AFS_File(Inode* inode, AFS* FS){
	//bind inode
	this->fileType = inode->type;
	this->inode = inode;
	this->inodeNum = inode->number;
	dataSize=0;
	usrData=NULL;
	this->FS = FS;//BUG! a constructor must ensure to initialize every field.
	this->offset=0;

	read_file_from_disk();
}

void AFS_File::write_file_to_disk(){
	if (fileType == 0x00){
	
		//serialize the latest map
		serialize_subdir_map();
		
		//size of the file already set. update the binded inode
		inode->size=dataSize;
		
		//Dir format
		//|parentInode(int)|fileNameLen(int)|fileNameStr|dataLen(int)|data
		//if file is a dir. write the subDirs to disk
		
		//get the position
		cout<<"getting the position"<<endl;
		int fileStartPos = FS->get_block_pos(inode->block[0]);
		
		//first write parentInode
		cout<<"writing parentInode"<<endl;
		FS->write_disk(fileStartPos,&parentInode,sizeof(int));
		fileStartPos+=sizeof(int);
		//then fileNameLen
		cout<<"writing fileName"<<endl;
		FS->write_disk(fileStartPos,&fileNameLen,sizeof(int));
		fileStartPos+=sizeof(int);
		//then fileNameStr
		cout<<"writing fileNameStr"<<endl;
		FS->write_disk(fileStartPos,fileName.c_str(),strlen(fileName.c_str()));//strlen does not count the terminating character
		fileStartPos+=strlen(fileName.c_str());
		//then len of data
		FS->write_disk(fileStartPos,&dataSize,sizeof(int));
		cout<<"dataSize "<<dataSize<<" written to disk"<<endl;
		fileStartPos+=sizeof(int);
		//then write the actual serialized map;
		cout<<"Writing serialized map"<<endl;
		FS->write_disk(fileStartPos,usrData,dataSize);
		cout<<"Writing file complete"<<endl;
	}
	else{
		//Regular file format
		//|fileNameLen(int)|fileNameStr|dataLen(int)|data
		//TODO:did not consider file would span several blocks for now
		//read the first block
		perror("Begin writing an regular file.\n");
		size_t fileStartPos = FS->get_block_pos(inode->block[0]);
		//put the fileName length
		FS->write_disk(fileStartPos,&fileNameLen,sizeof(int));
		fileStartPos+=sizeof(int);
		//put the fileName
		FS->write_disk(fileStartPos,fileName.c_str(),fileNameLen);
		fileStartPos+=fileNameLen;
		//put the dataSize
		FS->write_disk(fileStartPos,&dataSize,sizeof(int));
		fileStartPos+=fileNameLen;
		//put the data
		FS->write_disk(fileStartPos,usrData,dataSize);
	}
}
void AFS_File::serialize_subdir_map(){
	//this method serialize the subdir map of a dir and put the result (a c-style string) into usrData and set dataSize accordingly
			//serialize the map
		string buffer;
        auto mit = subDirs.begin();
        for (;mit!=subDirs.end();mit++){
        	char inodeNum[10];
        	sprintf(inodeNum,"%d",mit->second);
            buffer.append(mit->first+"::"+inodeNum);
			buffer.append("|");
        }
        
        //put buffer into the data area
        //char *tmpBuffer = (char*)malloc(buffer.length());
        
        if (dataSize!=0) free(usrData);
        usrData = (char*)malloc(buffer.length()+1);
        strcpy(usrData,buffer.c_str());//strcpy copies all characters in source to destination, INCLUDING the null terminating character.
        dataSize = buffer.length();
        /*
        cout<<"serialized map str: "<<buffer<<endl;
        cout<<"dataSize="<<dataSize<<endl;
        printf("usrData =%s\n",usrData);      
        */
}

void AFS_File::read_file_from_disk(){
    //for a dir type
	//assuming a dir file can be held by a single block
	
	if (fileType==0x00){
		//DIR file format
		//|parentInode(int)|fileNameLne(int)|fileNameStr|dataLen(int)|data
		FILE *pFile = fopen(AFS::DiskFileName.c_str(),"rb");

		assert(inode->blocks>0);
		
		size_t fileStartPos = FS->get_block_pos(inode->block[0]);
		
		fseek(pFile,fileStartPos,SEEK_SET);
		//get the parentInode number
		fread(&parentInode,sizeof(int),1,pFile);

		//get filenamelen
		fread(&fileNameLen,sizeof(int),1,pFile);
		//cout<<"In read_file_from_disk: fileNameLen="<<fileNameLen<<endl;
		
		char tmpFileName[30];
		assert(fileNameLen<30);

		//get filename
		fread(tmpFileName,sizeof(char),fileNameLen,pFile);
		tmpFileName[fileNameLen]='\0';
		fileName.assign(tmpFileName, fileNameLen); 
		//cout<<"In read_file_from_disk: fileName="<<fileName<<endl;

		//get dataSize
		fread(&dataSize,sizeof(int),1,pFile);
		
		assert(dataSize<4<<10);
		//printf("\nIn read_file_from_disk: len of file is: %d\n", len);
		//fileStartPos+=sizeof(int);
		usrData = (char*) malloc(dataSize+1);
		
		//get the content
		fread(usrData,sizeof(char),dataSize, pFile);//BUG: buffer is already pointer. do not put &buffer
		usrData[dataSize]='\0';
		//cout<<"In read_file_from_disk: usrData="<<string(usrData)<<endl;
		fclose(pFile);


		//parse buffer and put the elements in the vector of subDirs.::0|
		string bufferStr(usrData);
		//cout<<"In read_file_from_disk: bufferStr="<<bufferStr<<endl;
		
		int pos = 0;
		while (pos<bufferStr.length()-1){
			int delimPos = bufferStr.find("|",pos);
			string tempPair = bufferStr.substr(pos,delimPos-pos);
      		int pairDelimPos = tempPair.find("::");

			string key = tempPair.substr(0,pairDelimPos);

			int val = std::stoi(tempPair.substr(pairDelimPos+2));

      		subDirs.insert(make_pair(key,val));
			pos = delimPos+1;
		}
		//clean up memory
	}else{
		//Regular file format
		//|fileNameLen(int)|fileNameStr|dataLen(int)|data
		//TODO:did not consider file would span several blocks for now
		FILE *pFile = fopen(AFS::DiskFileName.c_str(),"rb");
		if (!pFile) {perror("Disk Operation Failure");exit(3);}

		//read the first block (to get some meta data)
		size_t fileStartPos = FS->get_block_pos(inode->block[0]);
		fseek(pFile,fileStartPos,SEEK_SET);
	
		//get the fileName length
		fread(&fileNameLen,sizeof(int),1,pFile);
		//get the fileName
		assert(fileNameLen<30);
		char tmpFileName[30];
		fread(&tmpFileName,sizeof(int),1,pFile);
		tmpFileName[fileNameLen]='\0';
		fileName.assign(tmpFileName,fileNameLen);
	
		//get dataSize
		fread(&dataSize,sizeof(int),1,pFile);
		
		//TODO:in prototyping. Only file less than 40K supported
		assert(dataSize<4<<10);
		
		//get the content
		usrData = (char*)malloc(dataSize+1);
		fread(usrData,1,dataSize,pFile);
		usrData[dataSize]='\0';
		fclose(pFile);
	}
}

AFS_File::~AFS_File(){
	cout<<"In AFS_File destructor\n";
	free(usrData);
}

void AFS_File::display_file_info(){
	assert(fileType==0x11);
	printf("Information about file %s", fileName.c_str());
	printf("File name: %s\nFile size: %d\nFile descriptor: %d\n",fileName.c_str(),dataSize,inode->number);
}

void AFS_File::display_dir_info(){
	if (fileType!=0x00) {
		perror("Cannot call the method with a non-directory type file.");
		return;
	}
	
	printf("\nFile name\t iNode number\n");
	for (auto mit=subDirs.begin();mit!=subDirs.end();mit++){
	 	printf("%s\t\t %d\n", mit->first.c_str(), mit->second);
	}
	printf("=======END=======\n");
}

bool AFS_File::file_exists(string name,map<string,int>::iterator &mit){
	mit = subDirs.find(name);
	return mit!=subDirs.end();
}
	

