# source/CMAES/test/Makefile

include ../../common.mk

TARGET ?= ackley

all: $(TARGET)

DIR_KORALI = $L../../source

LDXLIBS := -L$(DIR_KORALI) -lkorali $(LDXLIBS)  

CXXFLAGS += -I../../include

$(TARGET): $(TARGET).o
	$(LINK.o.xx) $^ $(LDXLIBS) $(CXXFLAGS)

$(TARGET).o: $(TARGET).cpp
	mpic++ -c $< $(CXXFLAGS)

test: $(TARGET)
	./$(TARGET)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
