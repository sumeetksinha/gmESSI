
CC= g++ -O3 -std=c++11
CFLAGS = -c

all: GmESSI

GmESSI: Element.o GmshParser.o GmshTranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o GmESSIPython.o GmESSI.o
		mkoctfile --link-stand-alone -I/usr/local/include/octave-3.8.0/octave  -lboost_python-py27 -lpython2.7  GmESSI.o Element.o GmshParser.o GmshTranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o GmESSIPython.o -o GmESSI
		mkoctfile -I/usr/local/include/octave-3.8.0/octave  -lboost_python-py27 -lpython2.7 Element.o GmshParser.o GmshTranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o GmESSIPython.o -o gmessi.so
		mv gmessi.so.oct gmessi.so

GmESSI.o: GmESSI.cpp
		$(CC) $(CFLAGS) GmESSI.cpp
GmshTranslator.o: GmshTranslator.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) GmshTranslator.cpp 
GmshParser.o: GmshParser.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) GmshParser.cpp
Mapping.o: Mapping.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) Mapping.cpp
PhysicalGroup.o: PhysicalGroup.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) PhysicalGroup.cpp
Node.o: Node.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) Node.cpp
Element.o: Element.cpp 
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) Element.cpp 
Semantics.o: Semantics.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) Semantics.cpp
Tokenizer.o: Tokenizer.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) Tokenizer.cpp
OctParser.o: OctParser.cpp
		mkoctfile -I/usr/local/include/octave-3.8.0/octave -c OctParser.cpp
GmESSIPython.o: GmESSIPython.cpp
		$(CC)  -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) GmESSIPython.cpp
clean:
		-rm *.o *.so GmESSI

install:
		if [ -d "/usr/local/GmESSI" ]; then	sudo rm -r /usr/local/GmESSI; fi
		echo "creating directory in usr/local/GmESSI"
		sudo mkdir /usr/local/GmESSI
		sudo mkdir /usr/local/GmESSI/src
		sudo mkdir /usr/local/GmESSI/bin
		sudo mkdir /usr/local/GmESSI/lib
		sudo cp *.cpp *.h mapping.fei Makefile /usr/local/GmESSI/src
		sudo cp mapping.fei GmESSI /usr/local/GmESSI/bin
		sudo cp gmessi.so mapping.fei /usr/local/GmESSI/lib
		if [ -d "/usr/lib/python2.7/GmESSI.so" ]; then sudo rm  /usr/lib/python2.7/gmessi.so ; fi
		if [ -d "/usr/lib/python2.7/mapping.fei" ]; then sudo rm  /usr/lib/python2.7/mapping.fei; fi
		sudo cp gmessi.so mapping.fei /usr/lib/python2.7	
		sudo ln -s -f /usr/local/GmESSI/bin/mapping.fei /usr/local/bin/mapping.fei
		sudo ln -s -f /usr/local/GmESSI/bin/GmESSI /usr/local/bin/GmESSI
		