#include "GmshTranslator.h"
#include <iostream>
#include <fstream>
#include <direct.h>

int main(int argc, char* argv[]){

	cout << argv[0]<< endl;
	if( argc == 1){ cout << "ERROR:: Please Enter the Gmsh File "<< endl; exit(0);}

	for (int i =1 ;i <argc ; i++){

	    string file = argv[i];
	    fstream InputFile(file,fstream::in);

	    if(!InputFile.is_open()){ cout << "ERROR:: The program failed to open the file = " <<  file << endl; exit(0);}

	    ofstream MainFile("/GmshFile/text.txt", fstream::out);
        fstream GeometryFile();
        fstream LoadFile();

        char* pwd[100]; _getwd(pwd);

        cout<< pwd;

	    // GmshTranslator Translator = GmshTranslator(file);
	    // Translator.Convert();
	}

	return 0;
}