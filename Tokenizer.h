/********************************************************************************************************
*  File:        Tokenizer.h          	     		 	| Copyright:: Sumeet Kumar Sinha			    *
*  Description: A general Tokenizer class       		| 513 I street                                  *
*  Rev:         Version 1                               | App# 207                              		*
*  Created:     June 28, 2015                           | Davis, California                             *
*  Author:      Sumeet Kumar Sinha                      | tel int 530-601-8271                          *
*  Email:       sumeet.kumar507@gmail.com               |                                               *
*                                                                                                       *
*  The copyright to the computer program(s) herein is the property of Sumeet Kumar Sinha, India.		*
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha      *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which      *
*  the program(s) have been supplied.                                                                   *
********************************************************************************************************/

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

using namespace::std;

const string DEFAULTDELIMITER =  " \t\v\n\r\f";

class Tokenizer{

	public:

		Tokenizer();
		Tokenizer( const string& str, const string& delm=DEFAULTDELIMITER);
		Tokenizer( vector<string>& strList, const string& str, const string& delm=DEFAULTDELIMITER);
		~Tokenizer();

		void set(const string& str, const string& delm=DEFAULTDELIMITER);
		void setString(const string& str);
		void setDelimiter(const string & delm);
		void setcurrPos(int pos);
		void setMode(int mode);
		string getString();
		int countTokens();
		bool hasMoreTokens();
		string currToken();
		string nextToken();
		string prevToken();
		vector<string> split();

	private:

		bool isDelimiter(char c);
		void skipDelimiters_forward();
		void skipDelimiters_backward();
		int nofTokens=0;
		int mode=0;
		vector<string> strList;
		string inpString;
		string Token;
		string strDelim;
		string::iterator currPos;
};

#endif //TOKENIZER_H