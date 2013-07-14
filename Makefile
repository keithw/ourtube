source = test1-httplisten.cc

objects =

executables = test1-httplisten

CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -pedantic -Werror -Wall -Wextra \
	-Weffc++ -fno-default-inline -pipe
LIBS =

all: $(executables)

test1-httplisten: test1-httplisten.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo
