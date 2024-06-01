SOURCES=$(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJECTS=$(patsubst src/%,build/%,${SOURCES:.cpp=.o})
DIRS=$(dir ${OBJECTS})

TESTSOURCES=$(wildcard tests/*.cpp)
TESTOBJECTS=$(patsubst tests/%,build/tests/%,${TESTSOURCES:.cpp=.o})
TESTDIRS=$(dir ${TESTOBJECTS})
TESTS=out/tests/binaryreader

CXX_FLAGS=-Wall -Wextra -Werror
LD_FLAGS=

NAME=pkmn_sprite
BINARY=out/${NAME}

.PHONY: all
all: dirs ${BINARY}

.PHONY: tests
tests: testdirs ${TESTS}

${BINARY}: ${OBJECTS}
	g++ ${LD_FLAGS} -o $@ $^

build/%.o: src/%.cpp
	g++ ${CXX_FLAGS} -o $@ -c $<

out/tests/binaryreader: build/tests/binaryreader.o build/gbimg/binaryreader.o
	g++ ${LD_FLAGS} -o $@ $^

build/tests/%.o: tests/%.cpp
	g++ ${CXX_FLAGS} -I./src/ -o $@ -c $<

.PHONY: dirs
dirs:
	mkdir -p build/ ${DIRS}
	mkdir -p out/

.PHONY: testdirs
testdirs:
	mkdir -p build/ ${DIRS} ${TESTDIRS}
	mkdir -p out/tests/

.PHONY: clean
clean:
	-rm -r build/
	-rm -r out/
