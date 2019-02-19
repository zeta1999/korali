include ../../../common.mk

LDFLAGS += -L$(DIR_KORALI) -lkorali
CXXFLAGS += -I../../../include

.SECONDARY: 
.PHONY: all 
all: auxiliar.o

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(BINARIES) *.o *.ti *.optrpt 


	
