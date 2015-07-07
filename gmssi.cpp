#include "GmshTranslator.h"

int main(argc, argv[]){

	if( argc == 1){ cout << "ERROR:: Please Enter the Gmsh File "<< endl; exit(0);}
    string file = argv[1];
    fstream InputFile(file,fstream::in);
    if(!InputFile.is_open()){ cout << "ERROR:: The program failed to open the file" << endl; exit(0);}

	return 0;
}