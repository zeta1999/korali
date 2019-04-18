include ../common.mk
CXXFLAGS += -I.
CXXFLAGS += -I../libs

SOURCES = $(shell find . -name "*.cpp")
OBJECTS = $(SOURCES:.cpp=.o)

all: libkorali.a

libkorali.a: $(OBJECTS)

clean:; rm -rf *.a *.o $(OBJECTS)

.PHONY: clean all
