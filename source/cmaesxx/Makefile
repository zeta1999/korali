# source/lib/camesxx/Makefile

include ../common.mk

TARGET=libcmaesxx.a

all: $(TARGET)

L=..

DIR_CMAES  = $L/cmaes

CXXFLAGS += -I$(DIR_CMAES) -I../korali

libcmaesxx.a: engine_cmaes.o 
engine_cmaes.o: engine_cmaes.cpp engine_cmaes.hpp engine_cmaes_utils.hpp

clean:; rm -rf $(TARGET) *.o *.a

.PHONY: clean all
