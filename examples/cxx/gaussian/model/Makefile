CXX = korali-cxx

all: gaussian.o

gaussian.o: gaussian.cpp
	$(CXX) -c $< -O3 

clean:; rm -rf $(TARGET) *.o

.PHONY: clean all
