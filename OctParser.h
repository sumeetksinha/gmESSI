/*****************************************************************************
* 								OctParser.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

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
