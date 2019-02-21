# compilers and flags

CXX = upcxx

CXXFLAGS += -O3 -g -std=c++17
#CXXFLAGS += -O0 -g -std=c++17
CXXFLAGS += -Wall -Wfatal-errors `gsl-config --cflags` -I/cluster/apps/intel/parallel_studio_xe_2018_r1/compilers_and_libraries_2018.1.163/linux/mpi/intel64/include
LDFLAGS += `gsl-config --libs` -lm 


COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(CXX) $(LDFLAGS)     -o $@

# rules

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
