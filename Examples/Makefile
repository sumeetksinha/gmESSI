
MSHFILE = PresentationExample.msh Foundation.msh 111.msh Contact.msh Dome.msh example.msh Example.msh Example3.msh Necking.msh
PYFILE = PresentationExample.py 111.py 

all: RunExamples

RunExamples: $(MSHFILE) $(PYFILE)
		gmessi $(MSHFILE)
		@echo "Sucessively completed checking of gmESSI Program"
		$(foreach var,$(PYFILE),python $(var);)
		python $(PYFILE)
		@echo "Sucessively checked gmESSI Python Module"
clean:
		rm -r *_ESSI_Simulation* 
		@echo "Sucessively removed all  ESSI_Simulation folders"