# source/CMAES/test/Makefile

include ../../source/common.mk

TARGET ?= ackley

all: $(TARGET)

L=../../source

DIR_KORALI = $L/korali

LDXLIBS := -L$(DIR_KORALI) -lkorali $(LDXLIBS)  

CXXFLAGS += -I$(DIR_KORALI)

$(TARGET): $(TARGET).o
	$(LINK.o.xx) $^ $(LDXLIBS) $(CXXFLAGS)

$(TARGET).o: $(TARGET).cpp
	mpic++ -c $< $(CXXFLAGS)

test: $(TARGET)
	./$(TARGET)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
