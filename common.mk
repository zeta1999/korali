#CXX = upcxx
CXX = icc
CXXFLAGS += -O3 -g -std=c++17
#CXXFLAGS += -O0 -pg -std=c++17
CXXFLAGS += -Wall -Wfatal-errors `gsl-config --cflags` -fopenmp
LDFLAGS += `gsl-config --libs` -lm -fopenmp

COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(CXX) $(LDFLAGS)     -o $@

# rules

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
