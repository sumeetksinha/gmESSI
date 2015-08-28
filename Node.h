/*********************************************************************************************************
*  File:        Node.h          	        			| Copyright:: Version of Creative Commons License*
*  Description: Contains Nodes data						| Boris Jeremic,                                 *
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
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <set>

#include "Tokenizer.h"

class Node{

	public:

		Node();
		Node(const int& id, const double& X_cord, const double& Y_cord, const double& Z_cord );
		Node(const string GmshNodeRep);
		~Node();
		void setId(const int& id);
		void setXcord(const double& X_cord);
		void setYcord(const double& Y_cord);
		void setZcord(const double& Z_cord);
		void setCord(const double& X_cord, const double& Y_cord, const double& Z_cord);
		void setNode(const int& id,const double& X_cord, const double& Y_cord, const double& Z_cord);
		void setNode(const string GmshNodeRep);
		int getId();
		double getXcord();
		double getYcord();
		double getZcord();

		//Just for Boost sake of 
		//Overloading of Operators

		bool operator==(const Node& other) {return false;}
  		bool operator!=(const Node& other) {return true;}
  		bool operator> ( Node& other ) { if(this->getId() >  other.getId()) return true; else return false;}
  		bool operator< ( Node& other ) { if(this->getId() <  other.getId()) return true; else return false;}
  		bool operator>=( Node& other ) { if(this->getId() >= other.getId()) return true; else return false;}
  		bool operator<=( Node& other ) { if(this->getId() <= other.getId()) return true; else return false;}

	private:

		int Id;
		double Xcord, Ycord, Zcord;
};

#endif //NODE_H