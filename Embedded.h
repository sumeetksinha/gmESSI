/*********************************************************************************************************
*  File:        Embedded.h          	        		| Copyright:: Version of Creative Commons License*
*  Description: For Embedding Files in Executable	 	| Boris Jeremic,                                 *
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
#ifndef EMBEDDED_H
#define EMBEDDED_H

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace::std;

class Embedded{

	public:

		Embedded();
		~Embedded();
		const char* ReadFile(string filename);;
};


#endif //EMBEDDED_H