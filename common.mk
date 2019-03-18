ifeq ($(KORALI_CONDUIT),mpi)
 KORALI_FLAGS = -DKORALI_MPI_CONDUIT
endif

ifeq ($(KORALI_CONDUIT),upcxx)
 KORALI_FLAGS = -DKORALI_UPCXX_CONDUIT
endif

ifeq ($(KORALI_CONDUIT),single)
 KORALI_FLAGS = -DKORALI_SINGLE_CONDUIT
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
