include ../../common.mk

LFLAGS = 

BINARIES = heat2d

.SECONDARY: 
.PHONY: all 
all: $(BINARIES)

heat2d: auxiliar/auxiliar.o heat2d.o 
	$(CC) $(CXXFLAGS) $(LFLAGS) -o $@ $^

auxiliar/auxiliar.o:
	$(MAKE) -C auxiliar

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -qopt-report=5

.PHONY: clean
clean:
	$(MAKE) -C auxiliar clean
	$(RM) $(BINARIES) *.o *.ti *.optrpt 


	
