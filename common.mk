# compilers and flags

CXX := mpic++
LDX := mpic++

CXXFLAGS += -O3 -std=c++11
CXXFLAGS += -Wall -Wfatal-errors
LDXLIBS += `gsl-config --libs` -lm

COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(LD)  $(LDFLAGS)     -o $@
LINK.o.xx   = $(LDX) $(LDFLAGS)     -o $@

# rules

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
