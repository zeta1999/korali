.PHONY: ackley nCandle sample

all: ackley nCandle sample

ackley:
	@$(MAKE) -j -C ackley

nCandle:
	@$(MAKE) -j -C nCandle

sample:	
	@$(MAKE) -j -C sample

clean: 
	@$(MAKE) -j -C ackley clean
	@$(MAKE) -j -C nCandle clean
	@$(MAKE) -j -C sample clean
