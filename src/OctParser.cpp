/*********************************************************************************************************
*  File:        OctaveParser.cpp                        | Copyright:: Version of Creative Commons License*
*  Description: Octave parser embedded in gmESSI        | Boris Jeremic,                                 *
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


#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include <octave/ov.h>
#include "OctParser.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace::std;

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

OctParser::OctParser(){

    // Disable warning printing from octave 
    std::ofstream error("/dev/null");
    std::streambuf *errbuf = std::cerr.rdbuf(error.rdbuf());

        char *oct_argv[3] = {(char*)"embeded",(char*)"-q",(char*)"--interactive"}; 
        octave_main(3,oct_argv,true);

    // Enable warning printing from octave 
    std:cerr.rdbuf(errbuf);

}

OctParser::~OctParser(){
}

/******************************************************************************
**************************** Public Functions *********************************
******************************************************************************/

string OctParser::eval(const string& function) {

    int status = 0;
    ofstream ScriptFile("script", ios::out);
    ScriptFile << function ;
    ScriptFile.close();
    source_file("script");
    //eval_string(function,false,status);
    octave_value f = eval_string("num2str(ans,10);",false,status);
    string val = f.string_value();
    return val; 
}


void OctParser::exit(){

	clean_up_and_exit (1);
	// reset();
}


