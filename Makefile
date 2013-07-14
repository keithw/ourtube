source = test1-httplisten.cc test2-httprequest.cc test3-serverclient.cc ezio.cc socket.cc address.cc

objects = ezio.o socket.o address.o

executables = test1-httplisten test2-httprequest test3-serverclient

CXX = g++
CXXFLAGS = -g -O3 -std=c++0x -pedantic -Werror -Wall -Wextra \
	-Weffc++ -fno-default-inline -pipe
LIBS =

all: $(executables)

test1-httplisten: test1-httplisten.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

test2-httprequest: test2-httprequest.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

test3-serverclient: test3-serverclient.o $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include depend

depend: $(source)
	$(CXX) $(INCLUDES) -MM $(source) > depend

.PHONY: clean
clean:
	-rm -f $(executables) depend *.o *.rpo
