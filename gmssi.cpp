#include "GmshTranslator.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string>

#ifdef _WIN32 
    #include <direct.h>
    #define GetCurrentDir _getcwd
    #define slash "\\"
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
	#define slash "/"
#endif

string getFilePath();

int main(int argc, char* argv[]){

	try{

		if( argc <= 1){ 

		    string msg = "\033[1;31mERROR:: Please Enter the Gmsh File \033[0m\n" ; 
		    throw msg.c_str();		
		}


		for (int i =1 ;i <argc ; i++){

		    string gmshFile = argv[i];
		    fstream InputFile (gmshFile,fstream::in);

		    if(!InputFile.is_open()){ 

		    	string msg = "\033[1;31mERROR:: The program failed to open the file = " +  gmshFile + " \033[0m\n" ; 
		    	throw msg.c_str();
		    	exit(0);
		    }

		    Tokenizer str = Tokenizer(gmshFile,".");
		    gmshFile = str.nextToken();
		    string newDirectory= getFilePath() + slash +gmshFile+ "_Essi_Simulation";

		    if(!str.nextToken().compare("msh")==0){

		    	string msg = "\033[1;31mERROR:: The file does not have .msh extension \033[0m\n" ; 
		    	throw msg.c_str();
		    }

		    if(!mkdir(newDirectory.c_str(),0777)==0){ 

		    	cout <<  "\033[1;31mERROR:: Directory Allready Present. Not Able to create the directory \"" + newDirectory  + "\" \033[0m\n";
		    	cout <<  "Do you want to override the folder contents?(Yy/Nn)" << endl << "\033[1;33mWARNING:: If you say yes, the contents of the Folder may get changed \033[0m\n" ;
		    	char response;cin >> response;

	    		if(response == 'n' ||response == 'N')  exit(0);
		    }
		    newDirectory  =newDirectory +slash;

		   	GmshTranslator Translator = GmshTranslator(gmshFile, newDirectory);
		    Translator.Convert();
		}
	} catch (const char* msg){cerr << msg << endl;}

	return 0;
}


string getFilePath(){

	char filePath[256];
	
	if(!GetCurrentDir(filePath, sizeof(filePath))){

		cout<<"** ERROR - Something went wrong, exiting...\n";
	}
	
	string FilePath(filePath);

	return FilePath;
}

bool copyFile(const string& Source, const string& Destination){

   	ifstream src(Source, ios::binary);
   	ofstream dest(Destination, ios::binary);
    dest << src.rdbuf();
    return src && dest;
}