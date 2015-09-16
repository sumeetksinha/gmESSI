/*******************************************************************************************
*
*
*
*
*
*
*******************************************************************************************/

Foundation_Thickness = 10;
Foundation_Size = 20;
Foundation_Transverse_Layers = 10;
Foundation_Depth_Layers = 10;
Soil_Block_Size = 100;
Soil_Depth = 100;
Soil_Layers_Below_Foundation = 10;
Soil_Extension_from_foundation = (Soil_Block_Size-Foundation_Size)/2;
Raised_Platform_thickness = 5;
Raised_Platform_layers = 2;

Point(1) = {-Foundation_Size/2,-Foundation_Size/2,0};
Extrude {Foundation_Size,0,0} {Point{1};Layers{Foundation_Transverse_Layers };}
Extrude {0,Foundation_Size,0} {Line{1};Layers{Foundation_Transverse_Layers };Recombine;}
NewSurface[] = Extrude {0,0,-Foundation_Thickness} {Surface{5};Layers{Foundation_Depth_Layers };Recombine;};

Dilate {{0, 0, 0}, 1.00005} { Duplicata { Surface{NewSurface[]};}}

Transfinite Line {29,30,31,32,34,39,44,49} = Foundation_Transverse_Layers+1;
Transfinite Line {35,37,40,45} = Foundation_Depth_Layers+1;

Soil_Surface_Bellow_Foundation = 28;

Physical Surface ("$NegY_Soil_Contact$") = {38};
Physical Surface ("$NegX_Soil_Contact$") = {33};
Physical Surface ("$PosY_Soil_Contact$") = {48};
Physical Surface ("$PosX_Soil_Contact$") = {43};
Physical Surface ("$Top_Soil_Contact$") =  {28};

Physical Surface ("$NegY_Foundation_Contact$") = {18};
Physical Surface ("$NegX_Foundation_Contact$") = {14};
Physical Surface ("$PosY_Foundation_Contact$") = {26};
Physical Surface ("$PosX_Foundation_Contact$") = {22};
Physical Surface ("$Bottom_Foundation_Contact$") = {27};

Physical Volume ("$Foundation_Volume$") = {1};
Extrude {0,0,Raised_Platform_thickness} {Surface{5};Layers{Raised_Platform_layers };Recombine;}
Physical Surface ("$Raised_Platform_Upper_Surface$") = {71};
Physical Volume ("$Raised_Platform_Volume$") = {2};

Transfinite Surface "*";
Recombine Surface "*";

NegYLine[] = {34};
NegXLine[] = {49};
PosYLine[] = {44};
PosXLine[] = {39};

//Extrusions are similarly done clockwise. So the numbering of line number follows clockwise rule starting from the bottom

	/********************************************** Building The Soil Surface  *******************************************************/

		index=0;
		NewEnteties[]=Extrude {0, -Soil_Extension_from_foundation, 0} {Line{NegYLine[]};Layers{Foundation_Transverse_Layers};Recombine;}; N=#NewEnteties[]/4;
		For i In {0:N-1}
			SoilUpperSurface[index]=NewEnteties [1+i*4]; index=index+1;
			NegYLine[i+1] = NewEnteties [i*4];
		EndFor

		NewEnteties[]=Extrude {-Soil_Extension_from_foundation,0, 0} {Line{NegXLine[],NewEnteties[N*4-1]};Layers{Foundation_Transverse_Layers};Recombine;}; N=#NewEnteties[]/4;
		For i In {0:N-2}
			SoilUpperSurface[index]=NewEnteties [1+i*4]; index=index+1;
			NegXLine[i+1] = NewEnteties [i*4];
		EndFor
		NegXLine[0] = NewEnteties [(N-1)*4-1];
		SoilUpperSurface[index]=NewEnteties [(N-1)*4]; 
		index=index+1;
		new = #NegYLine[]; NegYLine[new] = NewEnteties [(N-1)*4+3];

		NewEnteties[]=Extrude {0, Soil_Extension_from_foundation, 0} {Line{PosYLine[],NewEnteties[(N-1)*4-2]};Layers{Foundation_Transverse_Layers};Recombine;}; N=#NewEnteties[]/4;
		For i In {0:N-2}
			SoilUpperSurface[index]=NewEnteties [1+i*4]; index=index+1;
			PosYLine[i+1] = NewEnteties [i*4];
		EndFor
		PosYLine[0] = NewEnteties [(N-1)*4-1];
		SoilUpperSurface[index]=NewEnteties [(N-1)*4]; index=index+1;
		new = #NegXLine[]; NegXLine[new] = NewEnteties [(N-1)*4+3];

		NewEnteties[]=Extrude {Soil_Extension_from_foundation, 0,0} {Line{PosXLine[],NewEnteties[(N-1)*4-2]};Layers{Foundation_Transverse_Layers};Recombine;}; N=#NewEnteties[]/4;
		For i In {0:N-2}
			SoilUpperSurface[index]=NewEnteties [1+i*4]; index=index+1;
			PosXLine[i+1] = NewEnteties [i*4];
		EndFor
		PosXLine[0] = NewEnteties [(N-1)*4-1];
		SoilUpperSurface[index]=NewEnteties [(N-1)*4]; index=index+1;
		new = #PosYLine[]; PosYLine[new] = NewEnteties [(N-1)*4+3];

		NewEnteties[]=Extrude {0, -Soil_Extension_from_foundation, 0} {Line{NewEnteties[(N-1)*4-2]};Layers{Foundation_Transverse_Layers};Recombine;};
		NegYLine[0] = NewEnteties [0];
		SoilUpperSurface[index]=NewEnteties [1]; index=index+1; 
		new = #PosXLine[]; PosXLine[new] = NewEnteties [4];

		Physical Surface ("$SoilUpperSurface$") ={SoilUpperSurface[]};

/*###################################################### Building Soil Around The Foundation ###################################################*/

	index =0;SoilVolumeArraylength= 0;
	NewEnteties[] =  Extrude {0,0,-Foundation_Thickness} {Surface{SoilUpperSurface[]};Layers{Foundation_Depth_Layers};Recombine;}; N=#NewEnteties[]/6;
	For i In {0:N-1}
		SoilUpperSurface[i]=NewEnteties [i*6];
		SoilVolume[SoilVolumeArraylength+index] = NewEnteties [i*6+1];index=index+1; 
	EndFor

/*##############################################################################################################################################*/

/*###################################################### Building Soil Below The Foundation ###################################################*/

	index =0;SoilVolumeArraylength= #SoilVolume[];
	NewEnteties[] =  Extrude {0,0,-(Soil_Depth-Foundation_Thickness)} {Surface{SoilUpperSurface[],Soil_Surface_Bellow_Foundation};Layers{Soil_Layers_Below_Foundation};Recombine;}; N=#NewEnteties[]/6;
	For i In {0:N-1}
		SoilUpperSurface[i]=NewEnteties [i*6];
		SoilVolume[SoilVolumeArraylength+index] = NewEnteties [i*6+1];index=index+1; 
	EndFor

/*##############################################################################################################################################*/

Physical Volume ("$Soil_Volume$ ") ={SoilVolume[]};

