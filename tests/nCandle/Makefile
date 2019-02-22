include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
BINARIES = nCandle
CC = upcxx

.SECONDARY: 
.PHONY: all 
all: $(BINARIES)

nCandle: heat2d/heat2d.o nCandle.o
	$(CC) -o $@ $^ $(LDFLAGS)

heat2d/heat2d.o: heat2d/heat2d.o
	$(MAKE) -C heat2d heat2d.o

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C heat2d clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt 


	
