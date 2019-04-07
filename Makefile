.PHONY: all install clean
include common.mk

all: libkorali.a

libkorali.a:
	@$(MAKE) -j -C source

clean:
	@$(MAKE) -j -C source clean

install: libkorali.a
	@echo "[Korali] Installing Korali..."
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp source/libkorali.a $(PREFIX)/lib
