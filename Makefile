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
#  make help                                          -> show the available targets and options                           #
#  make                                               -> compiles gmessi                                                  #
#  make clean                                         -> cleans the object files                                          #
#  make install [OPTIONAL:: PREFIX=install_dir ]      -> installs gmessi in PREFIX=? directory                            #
#  make uninstall [OPTIONAL:: PREFIX=install_dir ]    -> builds individual libraries                                      #
#                                                                                                                         #
#  Sumeet Kumar Sinha (September,2016)                                                                                    #
#  Computational Geomechanics Group                                                                                       #
#  University of California, Davis                                                                                        #
#  s u m e e t k s i n h a . c o m                                                                                        #
###########################################################################################################################


PREFIX = /usr/local


all: gmessi

gmessi:##@Default Build gmESSI
		@cd ./src && make INSTALL_DIR=$(PREFIX)

clean:##@Default Clean the application build files
		@cd ./src && make clean INSTALL_DIR=$(PREFIX)

install:##@Default Install the application
		@cd ./src && make install INSTALL_DIR=$(PREFIX)

uninstall:##@Default Uninstall the application
		@cd ./src && make uninstall INSTALL_DIR=$(PREFIX)

help: ##@Default Show this help.
	@perl -e '$(HELP_FUN)' $(MAKEFILE_LIST)

	#PREFIX=<install_dir>            [Optional] Define the installation directory <install_dir>

HELP_FUN = \
	 %help; \
	 while(<>) { push @{$$help{$$2 // 'options'}}, [$$1, $$3] if /^(\w+)\s*:.*\#\#(?:@(\w+))?\s(.*)$$/ }; \
	 print "usage: make [target]\n\n"; \
	for (keys %help) { \
	 print "$$_:\n"; $$sep = " " x (20 - length $$_->[0]); \
	 printf("  %-30s %s\n", $$_->[0], $$_->[1]) for @{$$help{$$_}}; \
	 print "\n"; }  

