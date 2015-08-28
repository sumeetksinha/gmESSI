/*********************************************************************************************************
*  File:        OctaveParser.h          	        	| Copyright:: Version of Creative Commons License*
*  Description: Octave parser embedded in gmESSI		| Boris Jeremic,                                 *
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

#ifndef OCTPARSER_H
#define OCTPARSER_H

#include <string>

using namespace::std;

class OctParser{

	public:

		OctParser();
		~OctParser();

		string eval(const string& command);
		void exit();

	private:

};

#endif //OCTPARSER_H
