CCC = gcc
CXX = gcc

houghTransform: clean hough run

hough: src/hough.c
	gcc src/hough.c -o objs/hough.o `pkg-config --cflags --libs opencv` -lstdc++ -lm

run:
	./objs/hough.o

clean:
	rm -rf objs/*.o
