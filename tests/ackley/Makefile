include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
BINARIES = minimize_function
CC = upcxx

.SECONDARY:
.PHONY: all
all: $(BINARIES)

minimize_function: model/ackley.o minimize_function.o
	$(CC) -o $@ $^ $(LDFLAGS)

model/ackley.o: 
	$(MAKE) -C model all 

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C model clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt
