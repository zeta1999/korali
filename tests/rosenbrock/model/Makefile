include ../../../common.mk

all: rosenbrock.o

$(TARGET).o: $(TARGET).cpp
	$(CC) -c $< $(CXXFLAGS)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
