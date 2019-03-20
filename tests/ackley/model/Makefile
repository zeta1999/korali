include ../../../common.mk

all: ackley.o

$(TARGET).o: $(TARGET).cpp
	$(CXX) -c $< $(CXXFLAGS)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
