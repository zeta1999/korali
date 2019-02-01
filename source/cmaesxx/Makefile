# source/lib/camesxx/Makefile

include ../common.mk

TARGET=libcmaesxx.a

all: $(TARGET)

L=..

DIR_CMAES  = $L/cmaes

CXXFLAGS += -I. -I../korali

libcmaesxx.a: engine_cmaes.o cmaes.o 

clean:; rm -rf $(TARGET) *.o *.a

.PHONY: clean all
