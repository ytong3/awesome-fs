#include "trunk.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
	AFS myFs(20<<20);
	
	//myFs.format();
	perror("test complete.\n");

	//prompt user to input
	string args;
	cout<<">";
	while (getline(cin,args)){
		#ifdef DEGB
			cout<<"Command received: "<<args<<endl;
		#endif
		if (args=="exit") exit(0);
		myFs.process_command(args);
		cout<<">";
	}
}
