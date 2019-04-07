include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
CXXFLAGS += -I../../libs/json
BINARIES = minimize_function sample_function
OBJECTS = model/ackley.o

.SECONDARY:
.PHONY: all
all: $(BINARIES)

$(BINARIES) : % : %.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

model/ackley.o: 
	$(MAKE) -C model all 

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C model clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt *.txt
