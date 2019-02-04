# source/lib/cames/Makefile

include ../common.mk

CXXFLAGS += -I../include

all: libkorali.a

libkorali.a: base.o distribution.o dimension.o cmaes.o

clean:; rm -rf *.o *.a

.PHONY: clean all
