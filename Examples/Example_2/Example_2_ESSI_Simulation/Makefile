
# Default Makefile Variables 
NP=4 

all: 
	 essi -f /home/sumeet/sumeet.kumar507@gmail.com/git/gmESSI/Examples/Example_2/Example_2_ESSI_Simulation/Example_2_analysis.fei

serial: 
	 essi -f /home/sumeet/sumeet.kumar507@gmail.com/git/gmESSI/Examples/Example_2/Example_2_ESSI_Simulation/Example_2_analysis.fei

parallel: 
	 mpirun -np ${NP} pessi -f /home/sumeet/sumeet.kumar507@gmail.com/git/gmESSI/Examples/Example_2/Example_2_ESSI_Simulation/Example_2_analysis.fei

clean: 
	 rm *.feioutput 

