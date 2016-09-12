
###########################################################################################################################
#                                                                                                                         #
#  GMESSI :: Translator for The Real ESSI (Real Earthquake-Soil-Structure Interaction) Simulator                          #
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                                      #
#                                                                                                                         #
#  This makefile builds gmESSI Translator.                                                                                #
#  GITHUB:: https://github.com/SumeetSinha/gmESSI.git                                                                     #
#                                                                                                                         #
#  ***Building gmESSI makefile options ***                                                                                #
#                                                                                                                         #
#  make help                                               -> show the available targets and options                      #
#  make                                                    -> compiles gmessi                                             #
#  make clean                                              -> cleans the object files                                     #
#  make install [OPTIONAL:: INSTALL_DIR=install_dir ]      -> installs gmessi in INSTALL_DIR=? directory                  #
#  make uninstall [OPTIONAL:: INSTALL_DIR=install_dir ]    -> builds individual libraries                                 #
#                                                                                                                         #
#  Sumeet Kumar Sinha (September,2016)                                                                                    #
#  Computational Geomechanics Group                                                                                       #
#  University of California, Davis                                                                                        #
#  s u m e e t k s i n h a . c o m                                                                                        #
########################################################################################################################### 


CC= g++ -O3 -std=c++11
CFLAGS = -c

INSTALL_DIR = /usr/local

all: gmessi

gmessi: mapping.fei EmbeddFile Embedded.o Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o gmESSI.o  ##@Default Build gmESSI 
		mkoctfile --link-stand-alone -L/usr/bin -lboost_regex  -lboost_python-py27 -lpython2.7  Embedded.o gmESSI.o Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o -o gmessi
		mkoctfile -L/usr/bin -lboost_regex  -lboost_python-py27 -lpython2.7 Embedded.o Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o -o gmessi.so
		mv gmessi.so.oct gmessi.so
		cat success

gmESSI.o: gmESSI.cpp
		$(CC) $(CFLAGS) gmESSI.cpp
gmESSITranslator.o: gmESSITranslator.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) gmESSITranslator.cpp 
GmshParser.o: GmshParser.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) GmshParser.cpp
Mapping.o: Mapping.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) Mapping.cpp
PhysicalGroup.o: PhysicalGroup.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_regex-mt -lboost_python -lboost_system $(CFLAGS) PhysicalGroup.cpp
Node.o: Node.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) Node.cpp
Element.o: Element.cpp 
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) Element.cpp 
Semantics.o: Semantics.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) Semantics.cpp
Tokenizer.o: Tokenizer.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) Tokenizer.cpp
OctParser.o: OctParser.cpp
		mkoctfile -c OctParser.cpp
gmESSIPython.o: gmESSIPython.cpp
		$(CC)  -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) gmESSIPython.cpp
Embedded.o: EmbeddFile Embedded.cpp
		$(CC)  -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -fPIC -I/usr/include/python2.7  -lboost_regex  -lboost_python -lboost_system $(CFLAGS) Embedded.cpp
EmbeddFile: EmbeddFiles.cpp mapping.fei
		$(CC) EmbeddFiles.cpp -o EmbeddFiles
		./EmbeddFiles mapping.fei > Embedded.cpp

clean:##@Default Clean the application build files
		@-rm -rf *.o *.so gmessi ./EmbeddFiles Embedded.cpp

install:##@Default Install the application
		@ if [ -d "$(INSTALL_DIR)/gmESSI" ]; then	rm -r $(INSTALL_DIR)/gmESSI; fi
		@echo "creating directory in $(INSTALL_DIR)/gmESSI"
		@mkdir -p $(INSTALL_DIR)/gmESSI
		@mkdir -p $(INSTALL_DIR)/gmESSI/src
		@mkdir -p $(INSTALL_DIR)/gmESSI/bin
		@mkdir -p $(INSTALL_DIR)/gmESSI/lib
		@cp *.cpp *.h mapping.fei Makefile $(INSTALL_DIR)/gmESSI/src
		@cp gmessi EmbeddFiles $(INSTALL_DIR)/gmESSI/bin
		@cp gmessy.py $(INSTALL_DIR)/gmESSI/bin/gmessy
		@chmod +x $(INSTALL_DIR)/gmESSI/bin/gmessy
		@cp gmessi.so $(INSTALL_DIR)/gmESSI/lib
		@ if [ -d "/usr/lib/python2.7/gmessi.so" ]; then rm  /usr/lib/python2.7/gmessi.so ; fi
		@cp $(INSTALL_DIR)/gmESSI/lib/gmessi.so /usr/lib/python2.7	
		@ln -s -f $(INSTALL_DIR)/gmESSI/bin/gmessi /usr/local/bin/gmessi
		@ln -s -f $(INSTALL_DIR)/gmESSI/bin/gmessy /usr/local/bin/gmessy

uninstall:##@Default Uninstall the application
		@ if [ -d "$(INSTALL_DIR)/gmESSI" ]; then	rm -r $(INSTALL_DIR)/gmESSI; fi
		@ echo "removing directory in $(INSTALL_DIR)/gmESSI"
		@ if [ -d "/usr/lib/python2.7/gmessi.so" ]; then rm  /usr/lib/python2.7/gmessi.so ; fi
		@rm -rf /usr/local/bin/gmessi
		@rm -rf /usr/local/bin/gmessy

help: ##@Default Show this help.
	@perl -e '$(HELP_FUN)' $(MAKEFILE_LIST)

	#INSTALL_DIR=<install_dir>       [Optional] Define the installation directory <install_dir>

HELP_FUN = \
	 %help; \
	 while(<>) { push @{$$help{$$2 // 'options'}}, [$$1, $$3] if /^(\w+)\s*:.*\#\#(?:@(\w+))?\s(.*)$$/ }; \
	 print "usage: make [target]\n\n"; \
	for (keys %help) { \
	 print "$$_:\n"; $$sep = " " x (20 - length $$_->[0]); \
	 printf("  %-30s %s\n", $$_->[0], $$_->[1]) for @{$$help{$$_}}; \
	 print "\n"; }  

