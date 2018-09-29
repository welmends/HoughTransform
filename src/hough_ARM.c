/**
 * @file hough_ARM.c
 * @brief Standard Hough Transform Implemantation on C language
          Platform: ARM - Raspberry (arm-unknown-linux-gnueabi-gcc)

          Input : Path to an image, Type of the image (Canny or Original) and
                  Output type (Binary or 8 Bits)
          Output: Hough Transform Matrix (Accumulator image [PGM])

          Other: More information about the use of this code is present in
                 instructions method

 * @author $Author:$ de Souza, Joao Wellington Mendes; Brito, Messyo Sousa
 * @version $Revision:$
 * @date $Date:$ 29/09/2018
 */

 ///////////////////////////////////////////////////////////////////////////////
 //                                 Libraries                                 //
 ///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
//                                   Macros                                   //
////////////////////////////////////////////////////////////////////////////////
#define M_PI            (3.14159265358979323846)//PI value approximation
#define IMAGE_PATH_SIZE (100)//Standard size for image path
#define THRESH_VALUE    (200)//Standard thresh value
#define MAX_ROWS        (1920)//Maximum size for rows
#define MAX_COLS        (1080)//Maximum size for cols

////////////////////////////////////////////////////////////////////////////////
//                                  Structs                                   //
////////////////////////////////////////////////////////////////////////////////
typedef struct{
  char magicNumber[3];
  int rows,cols,grayscale;
  unsigned char pM[MAX_ROWS*MAX_COLS];
}Matrix;

////////////////////////////////////////////////////////////////////////////////
//                                 Prototypes                                 //
////////////////////////////////////////////////////////////////////////////////
void instructions(void);
void getImagePath(char **argv, char *imagePath, char *out_type);
void readImage(Matrix *image, const char *path);
void writeImage(Matrix *image, const char *path);
unsigned int _sqrt(int n);
int _ceil(float num);
float _sin(float x);
float _cos(float x);
void houghTransform(Matrix *image, Matrix *accumulator, char out_type);

////////////////////////////////////////////////////////////////////////////////
//                                Source Code                                 //
////////////////////////////////////////////////////////////////////////////////

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: The main method will call all other methods aiming to perform all
 *         operations prescribed. Also, the main method receives as input the
 *         arguments.
 * @param: int argc, char **argv
 * @return: int
 */
int main(int argc, char **argv) {
  if(argc!=4){
    instructions();
    return 1;
  }
  else{
    Matrix image[1];//image Matrix
    Matrix accumulator[1];//accumulator Matrix
    char imagePath[IMAGE_PATH_SIZE];//image path
    char out_type[1];//output type chosen
    struct timeval  tv1, tv2;//variables to time measurement

    gettimeofday(&tv1, NULL);
    getImagePath(argv,imagePath,out_type);
    readImage(image, imagePath);
    houghTransform(image, accumulator, *out_type);
    writeImage(accumulator, "results/houghAccu.pgm");
    gettimeofday(&tv2, NULL);

    printf ("Total time = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    return 0;
  }
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Print on screen the instructions to use this program.
 * @param: void
 * @return: void
 */
void instructions(void){
  printf("\n");
  printf(" **************************************************************************** \n");
  printf("|                               [Instructions]                               |\n");
  printf("| Compile: arm-unknown-linux-gnueabi-gcc src/hough_ARM.c -o objs/hough_ARM.o |\n");
  printf("| Command: ./objs/hough_ARM.o <type> <out_type> <image_name>                 |\n");
  printf("| Warning: The PGM image must be in 'images' folder                          |\n");
  printf("| Example: ./objs/hough_ARM.o canny binary lines                             |\n");
  printf("| Example: ./objs/hough_ARM.o original 8bits lines                           |\n");
  printf("|                                                                            |\n");
  printf("| <type>       : canny or original                                           |\n");
  printf("| <out_type>   : binary or 8bits                                             |\n");
  printf("| <image_name> : Any PGM image                                               |\n");
  printf("|                                                                            |\n");
  printf("| OBS: The output image will be generated in 'results' folder                |\n");
  printf(" **************************************************************************** \n");
  printf("\n");
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Verifies the type arguments, modifies a variable by adding the path to
 *         read a specific image and also modifies a variable of the output type.
 * @param: char **argv     - main arguments
           char *imagePath - variable to store image path
           char *out_type  - variable to store output type chosen
 * @return: void
 */
void getImagePath(char **argv, char *imagePath, char *out_type){
  // Verifies if the <type> and <out_type> params are valid
  if(strcmp(argv[1],"canny")!=0 && strcmp(argv[1],"original")!=0){
    printf("Error: <type> must be canny or original\n");
    exit(-1);
  }
  if(strcmp(argv[2],"binary")==0){
    *out_type='b';
  }
  else if(strcmp(argv[2],"8bits")==0){
    *out_type='8';
  }
  else{
    printf("Error: <out_type> must be binary or 8bits\n");
    exit(-1);
  }

  // Concatenate a path to the required image
  strcpy(imagePath, "images/");
  strcat(imagePath, argv[1]);
  strcat(imagePath, "/");
  strcat(imagePath, argv[3]);
  strcat(imagePath, ".pgm");
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Since the path to a specific image was passed to this method, this
 *         function reads the header and the PGM image and stores this at the
 *         Matrix Structure passed as input to this method.
 * @param: Matrix *image    - image Matrix to store in reading process
           const char *path - image path
 * @return: void
 */
void readImage(Matrix *image, const char *path){
  FILE *fp;//FILE variable

  // Open file in the provided path
  fp = fopen(path, "r");
  if(!fp){
    printf("Error: The image does not exist in the provided path\n");
    exit(-1);
  }

  // Read the magic number from the header
  fscanf(fp,"%s",&*image->magicNumber);

  // Discard comments
  while(getc((fp)) == '#'){
    while(getc((fp)) != '\n');
  }

  // Read the height, width and grayscale from the header
  fscanf(fp, "%d %d %d",&image->rows,&image->cols,&image->grayscale);

  // Read the image from the opened file
  fread(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

  // Close file
  fclose(fp);
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Writes the Matrix Structure passed as input to this method in a
 *         specific path that was also passed as input to this method.
 * @param: Matrix *image    - image Matrix to write in output image path
           const char *path - output image path
 * @return: void
 */
void writeImage(Matrix *image, const char *path){
    FILE *fp;//FILE variable

    // Open file in the provided path
    fp = fopen(path, "w");
    if(!fp){
      printf("Error: Could not open the file\n");
      exit(-1);
    }

    // Write the image in PGM format on the provided path
    fprintf(fp, "%s\n",image->magicNumber);
    fprintf(fp, "# PGM file generate by hough application\n");
    fprintf(fp, "%d %d\n",image->rows, image->cols);
    fprintf(fp, "%d\n",image->grayscale);
    fwrite(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

    // Close file
    fclose(fp);
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Calculates a square root of n from Newton's method.
 * @param: int n         - input
 * @return: unsigned int - output
 */
unsigned int _sqrt(int n) {
  unsigned int n1  = 1;
  unsigned int n2 = (((n1) + (n)/(n1)) >> 1);

  while(abs(n2 - n1) > 1) {
    n1 = n2;
    n2 = (((n) + (n)/(n1)) >> 1);
  }
  while(n2*n2 > n){
    n2--;
  }
  return n2;
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Calculates the ceil of n.
 * @param: int n - input
 * @return: int  - output
 */
int _ceil(float n) {
  int ceil_n = (int)n;
  if (n == (float)ceil_n) {
    return ceil_n;
  }
  return ceil_n + 1;
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Calculates the sine of x from the Taylor's series.
 * @param: float x - input
 * @return: float  - output
 */
float _sin(float x){
  return (x) -
         ((x*x*x)/(3*2*1.0)) +
         ((x*x*x*x*x)/(5*4*3*2*1.0)) -
         ((x*x*x*x*x*x*x)/(7*6*5*4*3*2*1.0)) +
         ((x*x*x*x*x*x*x*x*x)/(9*8*7*6*5*4*3*2*1.0)) -
         ((x*x*x*x*x*x*x*x*x*x*x)/(11*10*9*8*7*6*5*4*3*2*1.0));
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Calculates the cosine of x from the Taylor's series.
 * @param: float x - input
 * @return: float  - output
 */
float _cos(float x){
  return (1.0) -
         ((x*x)/(2*1.0)) +
         ((x*x*x*x)/(4*3*2*1.0)) -
         ((x*x*x*x*x*x)/(6*5*4*3*2*1.0)) +
         ((x*x*x*x*x*x*x*x)/(8*7*6*5*4*3*2*1.0)) -
         ((x*x*x*x*x*x*x*x*x*x)/(10*9*8*7*6*5*4*3*2*1.0));
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: This method performs all the calculus related to the Hough Transform.
 *         This occurs from the image scanning and calculation of the
 *         rho = xcos (theta) + ysin (theta) [0 < theta < 180] for pixels that
 *         display the high level (>THRESH_VALUE). It is worth mentioning that
 *         if the image passed as an input is not binarized, a threshold will
 *         be applied to it with a default threshold value set in THRESH_VALUE.
 *         More information: http://homepages.inf.ed.ac.uk/rbf/HIPR2/hough.htm
 *                           PATENT US3069654A - Paul V C Hough
 * @param: Matrix *image       - input image Matrix
           Matrix *accumulator - variable to store the accumulator
           char out_type       - output type chosen
 * @return: void
 */
void houghTransform(Matrix *image, Matrix *accumulator, char out_type){
  // Calculate the height and width of the Hough accumulator
  // accu_width  = 0 to 180 degrees
	// accu_height = 2*D - 1 and D = sqrt(height^2 + width^2)
	accumulator->rows        = 180;
  accumulator->cols        = _ceil(2*(_sqrt(image->rows*image->rows + image->cols*image->cols))) - 1;
  accumulator->grayscale   = image->grayscale;
  strcpy(accumulator->magicNumber,image->magicNumber);

  // Go to each pixel with hight level (>THRESH_VALUE) and calculate Rho to each Theta
	float rho;
	int theta,i,j;
	for(j=0; j<image->cols; j++){
		for(i=0; i<image->rows; i++){
			if(image->pM[ (j*image->rows) + i] > THRESH_VALUE){
				for(theta=0; theta<180; theta++){
					// rho = xcos(theta) + ysin(theta) [theta is in radians]
          rho = ( (j)*_cos((theta)*M_PI/180.0) ) + ( (i)*_sin(theta*M_PI/180.0) );

          if(out_type=='b'){
           // accumulator(theta,rho+D) = High Level
           accumulator->pM[ (int)((_ceil(rho + accumulator->cols/2) * 180.0)) + 180-theta-1] = accumulator->grayscale;
          }
          else{
           // accumulator(theta,rho+D)++
           accumulator->pM[ (int)((_ceil(rho + accumulator->cols/2) * 180.0)) + 180-theta-1]++;
          }
				}
			}
		}
	}
}
