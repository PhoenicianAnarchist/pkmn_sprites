SOURCES=$(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJECTS=$(patsubst src/%,build/%,${SOURCES:.cpp=.o})
DIRS=$(dir ${OBJECTS})

CXX_FLAGS=
LD_FLAGS=

NAME=pkmn_sprite
BINARY=out/${NAME}

.PHONY: all
all: dirs ${BINARY}

${BINARY}: ${OBJECTS}
	g++ ${LD_FLAGS} -o $@ $^

build/%.o: src/%.cpp
	g++ ${CXX_FLAGS} -o $@ -c $<

.PHONY: dirs
dirs:
	mkdir -p build/ ${DIRS}
	mkdir -p out/

.PHONY: clean
clean:
	-rm -r build/
	-rm -r out/
