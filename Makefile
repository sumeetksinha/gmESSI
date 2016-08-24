
CC= g++ -O3 -std=c++11
CFLAGS = -c

all: gmessi

gmessi: mapping.fei EmbeddFile Embedded.o Element.o GmshParser.o gmESSITranslator.o Mapping.o Node.o PhysicalGroup.o Semantics.o Tokenizer.o OctParser.o gmESSIPython.o gmESSI.o 
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
clean:
		-rm *.o *.so gmessi ./EmbeddFiles Embedded.cpp

install:
		if [ -d "/usr/local/gmESSI" ]; then	rm -r /usr/local/gmESSI; fi
		@echo "creating directory in usr/local/gmESSI"
		mkdir /usr/local/gmESSI
		mkdir /usr/local/gmESSI/src
		mkdir /usr/local/gmESSI/bin
		mkdir /usr/local/gmESSI/lib
		cp *.cpp *.h mapping.fei Makefile /usr/local/gmESSI/src
		cp gmessi EmbeddFiles /usr/local/gmESSI/bin
		cp gmessy.py /usr/local/gmESSI/bin
		cp gmessi.so /usr/local/gmESSI/lib
		if [ -d "/usr/lib/python2.7/gmessi.so" ]; then rm  /usr/lib/python2.7/gmessi.so ; fi
		cp gmessi.so /usr/lib/python2.7	
		ln -s -f /usr/local/gmESSI/bin/gmessi /usr/local/bin/gmessi
		ln -s -f /usr/local/gmESSI/bin/gmessy.py /usr/local/bin/gmessy
		chmod +x /usr/local/bin/gmessy
uninstall:
		if [ -d "/usr/local/gmESSI" ]; then	rm -r /usr/local/gmESSI; fi
		@echo "removing directory in usr/local/gmESSI"
		if [ -d "/usr/lib/python2.7/gmessi.so" ]; then rm  /usr/lib/python2.7/gmessi.so ; fi
		rm -f /usr/local/bin/gmessi
		rm -f /usr/local/bin/gmessy
		rm -f /usr/local/bin/gmessy.py
		