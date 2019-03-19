include ../common.mk
CXXFLAGS += -I../include

SOURCES =  $(shell ls *.cpp)
SOURCES += $(shell ls conduits/*.cpp)
SOURCES += $(shell ls solvers/*.cpp)
SOURCES += $(shell ls problems/*.cpp)
SOURCES += $(shell ls parameters/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: libkorali.a

libkorali.a: $(OBJECTS)

clean:; rm -rf *.a *.o $(OBJECTS)

.PHONY: clean all
