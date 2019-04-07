INCLUDES = $(shell cd source && find . | grep "\.h")

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
	mkdir -p $(PREFIX)/bin
	cp source/libkorali.a $(PREFIX)/lib
	cd source && cp $(INCLUDES) --parents $(PREFIX)/include
	cp libs/json -r $(PREFIX)/include
	@cat bin/korali-cxx | sed -e "s%(CXXFLAGS)%$(CXXFLAGS)%g" -e "s%(CXX)%$(CXX)%g" -e "s%(PREFIX)%$(PREFIX)%g" > $(PREFIX)/bin/korali-cxx
	@chmod a+x  $(PREFIX)/bin/korali-cxx
