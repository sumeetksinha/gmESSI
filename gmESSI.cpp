/*********************************************************************************************************
*  File:        PhysicalGroup.h          	        	| Copyright:: Version of Creative Commons License*
*  Description: Main gmESSI program 					| Boris Jeremic,                                 *
*  Rev:         Version 1                               | University of California Davis                 *
*  Created:     June 28, 2015                           | California                                     *
*  Author:      Sumeet Kumar Sinha                      | jeremic@ucdavis.edu                            *
*  Email:       sumeet.kumar507@gmail.com               |                                                *
*                                                                                                        *
*  The copyright to the computer program(s) herein is the property of Sumeet Kumar Sinha, Boris Jeremic  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha       *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which       *
*  the program(s) have been supplied.                                                                    *
*********************************************************************************************************/

#include "gmESSITranslator.h"
#include "gmESSIPython.h"
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

	int overwrite = 1, start =1; //atoi

	try{

		if( argc == 1){ 

		    string msg = "\033[1;31mERROR:: Please Enter the Gmsh File \033[0m\n" ; 
		    throw msg.c_str();		
		}

		string off = argv[1];

		if(!off.compare("-o")){
			overwrite =0;
			start =2;
		}

		for (int i =start ;i <argc ; i++){

		    string gmshFile = argv[i];
		    // cout << gmshFile << endl;
		    gmESSIPython gmssi = gmESSIPython (gmshFile);
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