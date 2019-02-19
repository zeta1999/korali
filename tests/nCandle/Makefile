include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
BINARIES = nCandle
CC = upcxx

.SECONDARY: 
.PHONY: all 
all: $(BINARIES)

nCandle: auxiliar/auxiliar.o heat2d.o nCandle.o
	$(CC) -o $@ $^ $(LDFLAGS)

auxiliar/auxiliar.o:
	$(MAKE) -C auxiliar 

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C auxiliar clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt 


	
