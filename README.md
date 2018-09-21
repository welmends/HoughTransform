# Hough Transform - C Implementation

Hough Transform Implementation in C language that provides the Hough Accumulator Matrix as output

### Prerequisites

  - GCC Compiler

### Compiling

- Open the terminal inside the folder

- Enter make command at terminal:

```shell
make
```
- or enter the gcc command yourself at terminal:

```shell
mkdir -p objs
gcc src/hough.c -o objs/hough.o -lm
```

### Executing

- Command:

```shell
./objs/hough.o <type> <out_type> <image_name>
```

- Warning: The PGM image must be in 'images' folder
