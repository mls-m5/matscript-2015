CPP_FILES= $(wildcard src/*.cpp) src/main/main.cpp

LIBS=
CXXFLAGS=-std=c++11 -I./ -Iinclude -Isrc


TARGET=matscript 
RUNSTRING=./${TARGET}       #it is possible to invoke "make run"

OBJECTS = $(CPP_FILES:.cpp=.o)    #Replaces cpp with object

#release target (standard)
release: CXXFLAGS += -Ofast
release: all

all: depend.mk ${TARGET} ${OBJECTS}

tests: all
	make -C tests

#debug configuration
debug: CXXFLAGS += -g -O0
debug: all

help:
	@cat INSTALL
	@echo

#Calculating dependencies
depend.mk: ${CPP_FILES}
	make -C matmake/
	@echo
	@echo Calculating dependencies
	matmake/matdep ${CPP_FILES} ${CXXFLAGS} > depend.mk

	
${TARGET}: ${OBJECTS}
	@echo linking
	${CXX} ${FLAGS} -o ${TARGET} ${OBJECTS} ${LIBS} $(FFT_ASM)

-include depend.mk

#För att kunna köra filen direkt
run: ${TARGET}
	${RUNSTRING}

clean:
	rm -f ${OBJECTS} ${ENGINE_OBJECTS}
	rm -f depend.mk
	make clean -C tests/

rebuild: clean ${TARGET}

deb:
	debuild -b

