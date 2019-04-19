BINARIES = maximize_likelihood maximize_posterior sample_likelihood sample_posterior
OBJECTS = model/libheat2d.so
CXX = korali-cxx

.SECONDARY:
.PHONY: all
all: $(BINARIES)

$(BINARIES) : % : %.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

model/%.so: 
	$(MAKE) -C model all 

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C model clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt *.txt
