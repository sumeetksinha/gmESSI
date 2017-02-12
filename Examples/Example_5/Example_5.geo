
// ###########################################################################################################################
// #                                                                                                                         #
// #  GMESSI :: Translator for The Real ESSI (Real Earthquake-Soil-Structure Interaction) Simulator                          #
// #  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                                      #
// #                                                                                                                         #
// #  Example_5 : Example to show how python module gets integrated with (.msh) mesh file,                                   #
// #                                                                                                                         #
// #  GITHUB:: https://github.com/SumeetSinha/gmESSI.git                                                                     #
// #                                                                                                                         #
// #  *** gmsh geometry file  for this example ***                                                                           #
// #                                                                                                                         #
// #  Sumeet Kumar Sinha (September,2016)                                                                                    #
// #  Computational Geomechanics Group                                                                                       #
// #  University of California, Davis                                                                                        #
// #  s u m e e t k s i n h a . c o m                                                                                        #
// ########################################################################################################################### 



Point(1) = {0,0,0};

// Extruding and subdividing  each dimension in 10 layers
Extrude{10,0,0}{Point{1}; Layers{10};}
Extrude{0,10,0}{Line{1}; Layers{10};Recombine;}
Extrude{0,0,10}{Surface{5}; Layers{10};Recombine;}

//Creating Physical Groups 
Physical Volume("$SoilVolume$")= {1};
Physical Surface("$Fixities$") = {5,26,18,14,22};
Physical Surface("$SurfaceLoad$") = {27};
