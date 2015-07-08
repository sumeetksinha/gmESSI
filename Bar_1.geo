
Lx=1; // X direction hole dimension
Ly=0.05; // Y direction hole dimension
Lz=0.25; // Length of the box
 
Nz=25;  // Number of Z subdivisions
Nx=100;   // Number of X subdivisions
Ny=5;   // Number of Y subdivisions
 
Point(1) = {0,0,0};  // point 1
 
Extrude{Lx, 0, 0} {Point{1}; Layers{Nx};}
Extrude{0, Ly, 0} {Line{1}; Layers{Ny};}

Transfinite Surface{5};
Recombine Surface{5};

Extrude{0,0,Lz} { Surface{5}; Layers{Nz}; Recombine;}

Translate{0,0,-0.26} {Volume{1};}

Physical Volume("$Soil$ <8NodeBrick{1}>") = {1};
Physical Surface("$Fix$ <FixNode{ux}>") = {26};

/**Physical Line("Longitudinal") = {1,2,9,7};
Physical Point("ongitudinal") = {1};
Physical Surface("Bar_1 back") = {22,14};
Physical Surface("Bar_1 front") = {5,27};
Physical Surface("Bar_1 face") = {26,18};
Physical Volume("Bar_1 soil") = {1};
Physical Point("Point") = {newp};
Physical Line("Line") = {newl};
Physical Surface("Surface") = {news};
Physical Volume("Volume") = {newv};**/




