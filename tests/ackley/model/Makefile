include ../../../common.mk

all: ackley.o

$(TARGET).o: $(TARGET).cpp
	$(CC) -c $< $(CXXFLAGS)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
