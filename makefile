CC = g++
CXXFLAGS=-std=c++11
LDLIBS   = -lpng
target = main

all: $(target)
clean:
	rm -f *.o *.png frames/*.png $(target)
main: image.o
