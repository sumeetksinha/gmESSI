
CC= g++ -O3 -std=c++11
CFLAGS = -c

all: gmssi

gmssi: Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o gmESSI.o
		mkoctfile --link-stand-alone -I/usr/local/include/octave-3.8.0/octave  -lboost_python-py27 -lpython2.7  gmESSI.o Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o -o gmessi
		mkoctfile -I/usr/local/include/octave-3.8.0/octave  -lboost_python-py27 -lpython2.7 Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o -o gmessi.so
		mv gmessi.so.oct gmessi.so

gmESSI.o: gmESSI.cpp
		$(CC) $(CFLAGS) gmESSI.cpp
gmESSITranslator.o: gmESSITranslator.cpp
		$(CC) -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) gmESSITranslator.cpp 
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
gmESSIPython.o: gmESSIPython.cpp
		$(CC)  -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -I/usr/local/boost_1_58_0 -I/usr/include/octave/ -lboost_python -lboost_system $(CFLAGS) gmESSIPython.cpp
clean:
		-rm *.o *.so gmessi

install:
		if [ -d "/usr/local/gmESSI" ]; then	sudo rm -r /usr/local/gmESSI; fi
		echo "creating directory in usr/local/gmESSI"
		sudo mkdir /usr/local/gmESSI
		sudo mkdir /usr/local/gmESSI/src
		sudo mkdir /usr/local/gmESSI/bin
		sudo mkdir /usr/local/gmESSI/lib
		sudo cp *.cpp *.h mapping.fei Makefile /usr/local/gmESSI/src
		sudo cp mapping.fei gmessi /usr/local/gmESSI/bin
		sudo cp gmessi.so mapping.fei /usr/local/gmESSI/lib
		if [ -d "/usr/lib/python2.7/gmessi.so" ]; then sudo rm  /usr/lib/python2.7/gmessi.so ; fi
		if [ -d "/usr/lib/python2.7/mapping.fei" ]; then sudo rm  /usr/lib/python2.7/mapping.fei; fi
		sudo cp gmessi.so mapping.fei /usr/lib/python2.7	
		sudo ln -s -f /usr/local/gmESSI/bin/mapping.fei /usr/local/bin/mapping.fei
		sudo ln -s -f /usr/local/gmESSI/bin/gmessi /usr/local/bin/gmessi
		