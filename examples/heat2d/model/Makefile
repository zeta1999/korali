all: libheat2d.so

CXX = korali-cxx
CXXFLAGS += -shared -march=native -D NOALIAS -O3 -fPIC -I../../../libs/  $(shell python3 -m pybind11 --includes)

libheat2d.so: heat2d.cpp 
	$(CXX) $^ -o $@ $(CXXFLAGS) 

clean:; rm -rf $(TARGET) *.so

.PHONY: clean all
