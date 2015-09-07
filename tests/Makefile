OBJECTS= ../src/sourcetree.o ../src/tokenizer.o

LIBS=
CXXFLAGS=-std=c++11 -I./ -Iinclude -Isrc -g -I../include -Og

all: tests

tests: sourcetree-test tokenizer-test

%-test: %-test.cpp $(OBJECTS)
	echo files: ${OBJECTS}
	g++ $< ${OBJECTS} ${CXXFLAGS} ${LIBS} -o $@
	
clean:
	rm -f *-test
	
