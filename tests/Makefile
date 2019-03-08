CASES = $(shell ls -d */)
.PHONY: $(CASES)

all: $(CASES)

$(CASES) : % : 
	@$(MAKE) -j -C $@

clean: 
	for i in $(CASES); do $(MAKE) -j -C $$i clean; done
