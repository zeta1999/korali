# compilers and flags

CXX = upcxx

#CXXFLAGS += -O3 -std=c++17
CXXFLAGS += -O0 -g -std=c++17
CXXFLAGS += -Wall -Wfatal-errors
LDFLAGS += `gsl-config --libs` -lm -L/opt/mpich/3.3/intel/2018.1/lib -lmpi


COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(CXX) $(LDFLAGS)     -o $@

# rules

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
