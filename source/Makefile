# source/lib/cames/Makefile

include ../common.mk

CXXFLAGS += -I../include

all: libkorali.a

libkorali.a: base.o problem.o distribution.o parameter.o cmaes.o

base.o: base.cpp
	upcxx -c $(CXXFLAGS) $<

clean:; rm -rf *.o *.a

.PHONY: clean all
