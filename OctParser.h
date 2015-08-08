/********************************************************************************************************
*  File:        OctaveParser.h           	        	| Copyright:: ##############################    *
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
