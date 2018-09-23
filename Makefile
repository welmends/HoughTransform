# Makefile

houghTransform: clean callShell hough hough_ARM

callShell:
	$(shell mkdir -p objs)

hough: src/hough.c
	gcc src/hough.c -o objs/hough.o -lm

hough_ARM: src/hough_ARM.c
	arm-unknown-linux-gnueabi-gcc src/hough_ARM.c -o objs/hough_ARM.o

run:
	./objs/hough.o

clean:
	rm -rf objs/*.o
