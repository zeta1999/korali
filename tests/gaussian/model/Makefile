include ../../../common.mk

all: gaussian.o

$(TARGET).o: $(TARGET).cpp
	$(CC) -c $< $(CXXFLAGS)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
