include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
BINARIES = minimize_function sample_function
OBJECTS = model/rosenbrock.o

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
