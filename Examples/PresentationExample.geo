
Point(1) = {0,0,0};
Extrude{10,0,0}{Point{1}; Layers{10};}
Extrude{0,10,0}{Line{1}; Layers{10};Recombine;}
Extrude{0,0,10}{Surface{5}; Layers{10};Recombine;}

Physical Volume("$SoilVolume$")= {1};
Physical Surface("$Fixities$") = {5,26,18,14,22};
Physical Surface("$SurfaceLoad$") = {27};
