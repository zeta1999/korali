# source/CMAES/test/Makefile

include ../../source/common.mk

TARGET ?= ackley

all: $(TARGET)

L=../../source

LIB_FITFUN ?= fitfun_tests

DIR_FITFUN  = ../../source/likelihoods
DIR_PRIORS  = $L/priors
DIR_CMAES   = $L/cmaes
DIR_CMAESXX = $L/cmaesxx
DIR_KORALI = $L/korali

LDXLIBS := -L$(DIR_PRIORS) -lpriors $(LDXLIBS)
LDXLIBS := -L$(DIR_CMAES) -lcmaes $(LDXLIBS)
LDXLIBS := -L$(DIR_CMAESXX) -lcmaesxx $(LDXLIBS)  
LDXLIBS := -L$(DIR_KORALI) -lkorali $(LDXLIBS)  
LDXLIBS := -L$(DIR_FITFUN) -l$(LIB_FITFUN) $(LDXLIBS)

CXXFLAGS += -I$(DIR_CMAES)
CXXFLAGS += -I$(DIR_CMAESXX)
CXXFLAGS += -I$(DIR_PRIORS)
CXXFLAGS += -I$(DIR_FITFUN)
CXXFLAGS += -I$(DIR_KORALI)

$(TARGET): $(TARGET).o
	$(LINK.o.xx) $^ $(LDXLIBS) $(CXXFLAGS)

$(TARGET).o: $(TARGET).cpp
	mpic++ -c $< $(CXXFLAGS)

test: $(TARGET)
	./$(TARGET)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
