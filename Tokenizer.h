/*****************************************************************************
* 								Tokenizer.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

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