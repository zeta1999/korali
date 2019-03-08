include ../../../common.mk

all: heat2d.o

CXXFLAGS += -march=native -D NOALIAS

$(TARGET).o: $(TARGET).cpp
	$(CC) -c $< $(CXXFLAGS)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
