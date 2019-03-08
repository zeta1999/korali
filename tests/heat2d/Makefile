include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
BINARIES = maximize_likelihood maximize_posterior sample_likelihood sample_posterior
OBJECTS = model/heat2d.o
CC = upcxx

.SECONDARY:
.PHONY: all
all: $(BINARIES)

$(BINARIES) : % : %.o $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

model/%.o: 
	$(MAKE) -C model all 

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C model clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt *.txt
