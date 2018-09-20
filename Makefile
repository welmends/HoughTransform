CCC = gcc
CXX = gcc

houghTransform: clean hough run

hough: src/hough.cpp
	gcc src/hough.cpp -o objs/hough.o `pkg-config --cflags --libs opencv` -lstdc++ -lm

run:
	./objs/hough.o

clean:
	rm -rf *.o
