#!/usr/bin/env python

""" Class for gmsh elements

It containts the information about GMSH Element
"""

__author__ = "Sumeet K. Sinha"
__credits__ = [""]
__license__ = "GPL"
__version__ = "2.0"
__maintainer__ = "Sumeet K. Sinha"
__email__ = "sumeet.kumar507@gmail.com"

class GmshNode:
	'It represents an REAL-ESSI element'

	__GmshTag               = -1;
	__EssiTag               = -1;
	__Coordinates           = -1;

	def __str__(self):
		return "\
\n---------------------------------------\n\
GmshNode  \n---------------------------------------\n\t\
GmshTag::      %s\n\t\
EssiTag::      %s\n\t\
Coordinates::  %s\n\t\
" % (self.__GmshTag,self.__EssiTag,self.Coordinates)

	def __init__ (self,GmshTag,Coordinates,EssiTag=-1):
		""" Define GmshNode class with output file

		Class Variables:
		GmshTag        -- GMSH tag for the element
		Coordinates    -- Coordinates in gmsh
		EssiTag        -- Element tag in REAL ESSI
		"""

		self.__GmshTag       = GmshTag;
		self.__Coordinates   = Coordinates;
		self.__EssiTag       = EssiTag;

	def __init__ (self,GmshNodeRep):

		try:

			Tokens = GmshNodeRep.split();
			Tokens = [float(i) for i in Tokens]

			self.__GmshTag = int(Tokens[0]);
			self.__Coordinates= Tokens[1:];

		except ValueError:
			print("GmshNode:: Gmsh File has invalid symbols in Element Section\
				Try again...")

	def GmshTag(self):
		""" return gmsh node tag """
		return self.__GmshTag;

	def EssiTag(self):
		""" return ESSI node tag """
		return self.__EssiTag;

	def Coordinates(self):
		""" return node coordinates """
		return self.__Coordinates;

# GmshNodeRep = "4 0.5 0.5 -0.5";
# X = GmshNode(GmshNodeRep);
# print X
