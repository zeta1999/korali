CXX = upcxx
CXXFLAGS += -O3 -g 
#CXXFLAGS += -O0 -g 
CXXFLAGS += -std=c++17 -Wall -Wfatal-errors `gsl-config --cflags` -fopenmp -D_KORALI_USE_UPCXX
LDFLAGS += `gsl-config --libs` -lm -fopenmp

COMPILE.cxx = $(CXX) $(CXXFLAGS) -c -o $@
LINK.o      = $(CXX) $(LDFLAGS)     -o $@

# rules

%.o: %.cpp
	$(COMPILE.cxx) $<

%.a:
	ar rcs $@ $^
	ranlib $@
