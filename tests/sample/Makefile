# source/CMAES/test/Makefile

include ../../common.mk

TARGET ?= sample

all: $(TARGET)

DIR_KORALI = $L../../source

LDFLAGS += -L$(DIR_KORALI) -lkorali 

CXXFLAGS += -I../../include

$(TARGET): $(TARGET).o
	upcxx -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

$(TARGET).o: $(TARGET).cpp
	upcxx -c $< $(CXXFLAGS)

test: $(TARGET)
	upcxx-run -n 1 ./$(TARGET)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
