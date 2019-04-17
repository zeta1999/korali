include ../common.mk
CXXFLAGS += -fPIC
CXXFLAGS += $(shell python3 -m pybind11 --includes)
CXXFLAGS += -I.
CXXFLAGS += -I../libs

SOURCES = $(shell find . | grep ".cpp")
OBJECTS = $(SOURCES:.cpp=.o)

all: libkorali.so

libkorali.so: $(OBJECTS)
	$(CXX) -shared $^ -o $@

clean:; rm -rf *.a *.o $(OBJECTS)

.PHONY: clean all
