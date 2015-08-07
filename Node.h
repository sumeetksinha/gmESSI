/*****************************************************************************
* 								Node.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/
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

		bool operator==(const Node& other) {return false;}
  		bool operator!=(const Node& other) {return true;}
		
	private:

		int Id;
		double Xcord, Ycord, Zcord;
};

#endif //NODE_H