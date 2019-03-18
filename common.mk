ifeq ($(KORALI_ENGINE),mpi)
 KORALI_FLAGS = -D_KORALI_MPI_ENGINE
endif

ifeq ($(KORALI_ENGINE),upcxx)
 KORALI_FLAGS = -D_KORALI_UPCXX_ENGINE
endif

ifeq ($(KORALI_ENGINE),single)
 KORALI_FLAGS = -D_KORALI_SINGLE_ENGINE
endif


CXX = upcxx
CXXFLAGS += -O3 -g -std=c++17
#CXXFLAGS += -O0 -pg -std=c++17
CXXFLAGS += -Wall -Wfatal-errors `gsl-config --cflags` $(KORALI_FLAGS)
LDFLAGS += `gsl-config --libs` -lm 

COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(CXX) $(LDFLAGS)     -o $@

# rules

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
