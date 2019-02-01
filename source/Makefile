# source/lib/cames/Makefile

include ../common.mk

CXXFLAGS += -I../include

all: libkorali.a

libkorali.a: korali.o distribution.o dimension.o 

clean:; rm -rf *.o *.a

.PHONY: clean all
