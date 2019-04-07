include ../../common.mk

LDFLAGS  += -L../../source -lkorali
CXXFLAGS += -I../../include
CXXFLAGS += -I../../libs/json
BINARIES = maximize_function sample_function
OBJECTS = model/gaussian.o

.SECONDARY:
.PHONY: all
all: $(BINARIES)

$(BINARIES) : % : %.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

model/%.o: 
	$(MAKE) -C model all 

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C model clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt *.txt
