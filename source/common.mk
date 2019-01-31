# compilers and flags

use_torc?=0
use_omp?=0

CC  := mpicc
LD  := mpicc
CXX := mpic++
LDX := mpic++

#CXXFLAGS += -O3 -std=c++11
CXXFLAGS += -O0 -g -std=c++11
CXXFLAGS += -Wall -Wno-unused-function -Wfatal-errors

LDLIBS += `gsl-config --libs` -lm -lpthread
LDXLIBS += `gsl-config --libs` -lm -lpthread

COMPILE.c   = $(CC)  $(CFLAGS)   -c -o $@
COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(LD)  $(LDFLAGS)     -o $@
LINK.o.xx   = $(LDX) $(LDFLAGS)     -o $@

# rules

%.o: %.c
	$(COMPILE.c) $<

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
