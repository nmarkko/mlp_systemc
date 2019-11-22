SRC_CPP=$(wildcard *.cpp)
SRC_HPP=$(wildcard *.hpp)
MAIN_TARGET=main*.cpp
TARGET=$(patsubst %.cpp,%,$(wildcard main*.cpp))
all: $(TARGET)
$(TARGET): $(SRC_CPP) $(SRC_HPP)
	g++  -std=c++14 -DSC_INCLUDE_FX -w -g -I ${SYSTEMC}/include -L${SYSTEMC}/lib-cygwin -lsystemc -o $@ *.cpp
.PHONY: clean
clean:
	rm $(TARGET) 
