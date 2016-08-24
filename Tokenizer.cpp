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

#include "Tokenizer.h"
#include <iostream>
#include <algorithm>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Tokenizer::Tokenizer(){

	inpString="";
	Token="";
	strDelim="";
	currPos=inpString.begin();
	mode = 0;
	nofTokens=0;
}

Tokenizer::Tokenizer(const string& str, const string& delm){

	inpString=str;
	Token="";
	strDelim=delm;
	currPos=inpString.begin();
	mode = 0;
	nofTokens=0;
}

Tokenizer::Tokenizer(vector<string>& strList, const string& str, const string& delm){

	inpString=str;
	Token="";
	strDelim=delm;
	strList= this->split();
	currPos=inpString.begin();
	mode = 0;
	nofTokens=0;
}

Tokenizer::~Tokenizer(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void Tokenizer::set(const string& str, const string& delm){

	this->inpString=str;
	this->strDelim=delm;
	this->currPos=this->inpString.begin();	
	mode = 0;
	nofTokens=0;
}

void Tokenizer::setString(const string& str){

	this->inpString=str;
	this->currPos=this->inpString.begin();
	mode = 0;
	nofTokens=0;
}

void Tokenizer::setDelimiter(const string& delm){

	this->strDelim = delm;
}

void Tokenizer::setcurrPos(int pos){

	this->currPos=(this->inpString.begin())+pos;
}

void Tokenizer::setMode(int Mode){

	this->mode=Mode;
}

string Tokenizer::getString(){

	return this->inpString;
}

int Tokenizer::countTokens(){

	this->split();
	return this->nofTokens;
}

bool Tokenizer::hasMoreTokens(){

	if(!mode)
	{
		if (this->currPos >= this->inpString.end()) 
			return false;
	}
	else
	{
		if (this->currPos <= this->inpString.begin()) 
			return false;
	}
	return true;
}

string Tokenizer::currToken(){

	return this->Token;
}

string Tokenizer::nextToken(){

	if(!mode)
	{
		if(this->inpString.length()<=0) return "";
		
		this->Token="";
	    this->skipDelimiters_forward(); 

	    while(this->currPos != this->inpString.end() && !isDelimiter(*(this->currPos))){

	    	this->Token =this->Token + *(this->currPos);
	        this->currPos=this->currPos+1;
	    }

	    this->currPos=this->currPos+1;
	}
	else
	{
		this->setMode(0);
		this->Token=this->prevToken();
		this->setMode(1);

	} 

    return this->Token; 
}

string Tokenizer::prevToken(){

	if(!mode)
	{
		if(this->inpString.size()<=0) return "";
		
		this->Token="";
	    this->skipDelimiters_backward();

	    while(this->currPos >= this->inpString.begin() && !isDelimiter(*(this->currPos))){

	    	this->Token =this->Token + *(this->currPos);
	        this->currPos=this->currPos-1;
	    }

	    this->currPos=this->currPos-1;

	    string temp = this->Token;
	    this->Token="";
	      
	    for(string::iterator i=temp.end();i>=temp.begin();i--)
	    		this->Token =this->Token + *(i);
	    this->Token = this->Token.substr(1,this->getString().length()-1);
	}
	else
	{
		this->setMode(0);
		this->Token=this->nextToken();
		this->setMode(1);
	} 

	return this->Token; 

}

vector<string> Tokenizer::split(){

	string::iterator pos = this->currPos;
	nofTokens =0;
	this->strList.clear();
	if(!mode)
	{
		while(this->currPos < this->inpString.end()){

			nofTokens =nofTokens+1;
			strList.push_back(this->nextToken());
		}
	}
	else
	{
		while(this->currPos > this->inpString.begin()){

			nofTokens =nofTokens+1;
			strList.push_back(this->nextToken());
		}

		reverse(this->strList.begin(), this->strList.end());
	}

	this->currPos= pos;

	return this->strList;
}

int Tokenizer::currIndex(){

	return std::distance(inpString.begin(), this->currPos);
}

/******************************************************************************
****************************** Private Function*********************************
******************************************************************************/

bool Tokenizer::isDelimiter(char c){

	return (strDelim.find(c) != string::npos);
}

void Tokenizer::skipDelimiters_forward()
{
    while(currPos !=this->inpString.end() && isDelimiter(*currPos))
        ++currPos;
}

void Tokenizer::skipDelimiters_backward()
{
    while(currPos != this->inpString.begin() && isDelimiter(*currPos))
        --currPos;
}

