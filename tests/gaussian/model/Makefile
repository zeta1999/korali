include ../../../common.mk

all: gaussian.o

$(TARGET).o: $(TARGET).cpp
	$(CXX) -c $< $(CXXFLAGS)

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
