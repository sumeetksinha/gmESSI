// Generating mesh For to simulate Necking in Steel Bar in ESSI 

Point(1) ={0,0,0};
Extrude{1,0,0}{Point{1};Layers{{10,20},{0.1,1}};}
Extrude{0,0.05,0}{Line{1};Layers{5};}

Transfinite Surface{5};
Recombine Surface{5};
Extrude{0,0,0.25}{Surface{5};Layers{5};Recombine;}

Physical Surface("$-XFace$ <Fix{ux}> <AddAllNode{m,3}>")={26};
Physical Surface("$+XFace$ <Fix{uz}> <MasterSlave{2,ux}>")={18};
Physical Surface("$-YFace$ <Fix{uz}>")={14};
Physical Surface("$+YFace$")={22};
Physical Surface("$-ZFace$ <Fix{uy}>")={5};
Physical Surface("$+ZFace$")={27};
Physical Volume("$soil1$ <8NodeBrick{1}>" )={1}; 

View "comments" {
  // Add a text string in window coordinates, 10 pixels from the left
  // and 10 pixels from the bottom:
  T2(10, -10, 0){ "Copyright (C) Sumeet Kumar Sinha" };

  // Add another text string in window coordinates, 10 pixels from the
  // left and 15 pixels from the top, using the StrCat() function to
  // concatenate a string with the current date: <AddNode{units,dofs}>
  T2(10, 15, 0){ StrCat("File created on ", Today) };

  // Add a text string in model coordinates at (X,Y,Z) = (0, 0.11, 0):
  // T3(0, 0.11, 0, 0){ "Hole" };
};

// name: gmssi
//  // scopeName: source.gmssi 
// fileTypes: [msh, geo]
// uuid: b095c611-75c4-4280-af1f-72c1cac74826

// /*# [PackageDev] target_format: plist, ext: tmLanguage */

//  patterns:

// Coherence;