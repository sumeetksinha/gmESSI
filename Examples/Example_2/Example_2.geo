
// ###########################################################################################################################
// #                                                                                                                         #
// #  GMESSI :: Translator for The Real ESSI (Real Earthquake-Soil-Structure Interaction) Simulator                          #
// #  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                                      #
// #                                                                                                                         #
// #  Example_1 : Modelling of Cantilever Beam With Uniformly Distributed Load                                               #
// #                                                                                                                         #
// #  GITHUB:: https://github.com/SumeetSinha/gmESSI.git                                                                     #
// #                                                                                                                         #
// #  *** gmsh geometry file  for this example ***                                                                           #
// #                                                                                                                         #
// #  Sumeet Kumar Sinha (Decmbere,2016)                                                                                     #
// #  Computational Geomechanics Group                                                                                       #
// #  University of California, Davis                                                                                        #
// #  s u m e e t k s i n h a . c o m                                                                                        #
// ########################################################################################################################### 

// Creating a point
Point(1) = {0,0,0};  

// Dividing the beam length in 5 parts                            
Extrude (4,0,0) {Point{1}; Layers{5};}  

// Dividing the beam width in 2 parts           
Extrude (0,1,0) {Line{1}; Layers{2};Recombine;}

// Dividing the beam depth in 2 parts
Extrude (0,0,1) {Surface{5}; Layers{2};Recombine;} 

// Generating Physical Groups

Physical Point ("All_Points") ={1,2,3,4,5,6,10,14};
Physical Surface("All_Surfaces") = {5,14,22,27,18,26};
Physical Line("All_Lines") ={1,2,3,4,12,13,21,17,7,8,9,10};
Physical Volume("All_Volumes") ={1};
Physical Surface("ApplySurfaceLoad") ={27};
Physical Surface("SurfaceToBeFixed") ={26};