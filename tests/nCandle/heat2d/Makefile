include ../../../common.mk

LDFLAGS += -L$(DIR_KORALI) -lkorali
CXXFLAGS += -I../../../include -march=native -D NOALIAS

.SECONDARY: 
.PHONY: all 
all: auxiliar.o heat2d.o

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(BINARIES) *.o *.ti *.optrpt 


	
