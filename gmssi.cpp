/********************************************************************************************************
*  File:        gmssi.cpp           	        		| Copyright:: ##############################    *
*  Description: Contains the magic of translation       | BOX 1505                                      *
*  Rev:         Version 1                               | 125 25 ALVSJO                                 *
*  Created:     June 28, 2015                           | SWEDEN                                        *
*  Author:      Sumeet Kumar Sinha                      | tel int 530-601-8271                          *
*  Email:       sumeet.kumar507@gmail.com               |                                               *
*                                                                                                       *
*  The copyright to the computer program(s) herein is the property of #######################, ######.  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha      *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which      *
*  the program(s) have been supplied.                                                                   *
********************************************************************************************************/

#include "GmshTranslator.h"
#include "PythonInterpreter.h"
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

	int override = 1, start =1; //atoi
	string off = argv[1];

	try{

		if(!off.compare("-o")){
			override =0;
			start =2;
		}

		if( argc <= 1){ 

		    string msg = "\033[1;31mERROR:: Please Enter the Gmsh File \033[0m\n" ; 
		    throw msg.c_str();		
		}


		for (int i =start ;i <argc ; i++){

		    string gmshFile = argv[i];
		    fstream InputFile (gmshFile,fstream::in);

		    if(!InputFile.is_open()){ 

		    	string msg = "\033[1;31mERROR:: The program failed to open the file = " +  gmshFile + " \033[0m\n" ; 
		    	throw msg.c_str();
		    	exit(0);
		    }

		    Tokenizer str = Tokenizer(gmshFile,"/ .");
		    str.setcurrPos(gmshFile.length()-1); str.setMode(1); str.nextToken();gmshFile = str.nextToken();
		    string newDirectory= getFilePath() + slash + gmshFile+ "_Essi_Simulation";
		    gmshFile = argv[i];

		    int n = 1;string tempDirectory = newDirectory;



	    	while(!mkdir(newDirectory.c_str(),0777)==0){ 

		    	if(override>=1){ 
		    		cout << "\033[1;36mFiles will be converted to " << newDirectory << "  \033[0m\n" << endl; 
		    		cout << "\033[1;33mWARNING::Directory Allready Present. The contents of the Folder may get changed \033[0m\n"; 
		    		break;
		    	}
		    	else{ 
		    		newDirectory = tempDirectory+"_"+to_string(n); 
		    		n=n+1;
		    	}
	    	}

	    	cout << "\033[1;36mFiles converted to " << newDirectory << "  \033[0m\n" << endl; 
		    newDirectory  =newDirectory +slash;

		    PythonInterpreter gmssi = PythonInterpreter ();
		    gmssi.Translator = GmshTranslator(gmshFile, newDirectory);
		    gmssi.Translator.Convert();

		    gmssi.Translator.UpdateNewTagNumbering();
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