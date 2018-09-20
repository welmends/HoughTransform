CCC = gcc
CXX = gcc

houghTransform: clean callShell hough run

callShell:
	$(shell mkdir -p objs)

hough: src/hough.c
	gcc src/hough.c -o objs/hough.o -lm

run:
	./objs/hough.o

clean:
	rm -rf objs/*.o
