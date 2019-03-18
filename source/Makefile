include ../common.mk

CXXFLAGS += -I../include
SOURCES = $(shell ls *.cpp) $(shell ls conduits/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: libkorali.a

libkorali.a: $(OBJECTS)

clean:; rm -rf *.a $(OBJECTS)

.PHONY: clean all
