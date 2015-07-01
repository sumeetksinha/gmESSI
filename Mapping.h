/*****************************************************************************
* 								Mapping.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/
#ifndef MAPPING_H
#define MAPPING_H

#include <map>
#include <list>
#include <queue>
#include <set>

#include "Tokenizer.h"
#include "Semantics.h"

const string DEFAULTMAPFILE = "Mapping.fei";

class Mapping{

	public:

		Mapping();
		Mapping(const string &str=DEFAULTMAPFILE);
		~Mapping();

	private:

		map<string,Semantics> Function;
		set<string> EssiTagList;
};

#endif //MAPPING_H