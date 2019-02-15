include ../../../common.mk

.SECONDARY: 
.PHONY: all 
all: auxiliar.o

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $< -Wno-unknown-pragmas

.PHONY: clean
clean:
	$(RM) $(BINARIES) *.o *.ti *.optrpt 


	
