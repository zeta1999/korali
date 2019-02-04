all:
	@$(MAKE) -j -C source
	@$(MAKE) -j -C tests

clean:
	@$(MAKE) -j -C source clean
	@$(MAKE) -j -C tests clean

test:
	upcxx-run -n 8 ./tests/ackley/ackley
