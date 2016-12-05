CXX = g++
CXXFLAGS = -std=c++14 -Wall
EXEC = cc3k
FILES = $(wildcard */*.cc) $(wildcard *.cc)

${EXEC}: $(FILES)
	${CXX} ${CXXFLAGS} ${FILES} -o ${EXEC} -lncurses -lpanel -lmenu

.PHONY: clean

clean:
	rm ${FILES} ${EXEC}
