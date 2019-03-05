# source/lib/cames/Makefile

include ../common.mk

CXXFLAGS += -I../include

all: libkorali.a

libkorali.a: problem.o distribution.o parameter.o cmaes.o tmcmc.o tmcmc_utils.o

base.o: base.cpp
	upcxx -c $(CXXFLAGS) $<

clean:; rm -rf *.o *.a

.PHONY: clean all
