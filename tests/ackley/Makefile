include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
BINARIES = minimize_function sample_function
OBJECTS = model/ackley.o
CC = upcxx

.SECONDARY:
.PHONY: all
all: $(BINARIES)

$(BINARIES) : % : %.o $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

model/ackley.o: 
	$(MAKE) -C model all 

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C model clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt
