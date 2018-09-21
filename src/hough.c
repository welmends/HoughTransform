/**
 * @file hough.c
 * @brief Hough Transform Implemantation
          Input : Path to an image, Type of the image (Canny or Original) and
                  Output type (Binary or 8 Bits)
          Output: Hough Transform Matrix (Accumulator image [PGM])
 * @author $Author:$ de Souza, Joao Wellington Mendes; Brito, Messyo Sousa
 * @version $Revision:$
 * @date $Date:$ 21/09/2018
 */

 ////////////////////////////////////////////////////////////////////////////////
 //                                 Libraries                                  //
 ////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//                                   Macros                                   //
////////////////////////////////////////////////////////////////////////////////
#define IMAGE_PATH_SIZE (100)
#define THRESH_VALUE    (200)

////////////////////////////////////////////////////////////////////////////////
//                                  Structs                                   //
////////////////////////////////////////////////////////////////////////////////
typedef struct{
  char magicNumber[3];
  int rows,cols,grayscale;
  unsigned char *pM;
}Matrix;

////////////////////////////////////////////////////////////////////////////////
//                                 Prototypes                                 //
////////////////////////////////////////////////////////////////////////////////
void instructions(void);
char* getImagePath(char **argv, char *out_type);
void readImage(Matrix *image, const char *path);
void writeImage(Matrix *image, const char *path);
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
    Matrix *image       = (Matrix*)calloc(1,sizeof(Matrix));
    Matrix *accumulator = (Matrix*)calloc(1,sizeof(Matrix));
    char out_type[1];

    readImage(image, getImagePath(argv, out_type));
    houghTransform(image, accumulator, *out_type);
    writeImage(accumulator, "results/houghAccu.pgm");

    free(image->pM);
    free(image);
    free(accumulator->pM);
    free(accumulator);
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
  printf(" ********************************************************* \n");
  printf("|                      [Instructions]                     |\n");
  printf("| Compile: gcc src/hough.c -o objs/hough.o -lm            |\n");
  printf("| Command: ./objs/hough.o <type> <out_type> <image_name>  |\n");
  printf("| Warning: The PGM image must be in 'images' folder       |\n");
  printf("| Example: ./objs/hough.o canny binary lines              |\n");
  printf("| Example: ./objs/hough.o original 8bits lines            |\n");
  printf("|                                                         |\n");
  printf("| <type>       : canny or original                        |\n");
  printf("| <out_type>   : binary or 8bits                          |\n");
  printf("| <image_name> : Any PGM image                            |\n");
  printf(" **************************************************** \n");
  printf("\n");
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Verifies the type arguments and returns a path to read a specifc
 *         image.
 * @param: char **argv, char *out_type
 * @return: char*
 */
char* getImagePath(char **argv, char *out_type){
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
  char *imagePath = (char*)calloc(IMAGE_PATH_SIZE, sizeof(char));
  strcpy(imagePath, "images/");
  strcat(imagePath, argv[1]);
  strcat(imagePath, "/");
  strcat(imagePath, argv[3]);
  strcat(imagePath, ".pgm");

  return imagePath;
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Since the path to a specific image was passed to this method, this
 *         function reads the header and the PGM image and stores this at the
 *         Matrix Structure passed as input to this method.
 * @param: Matrix *image, const char *path
 * @return: void
 */
void readImage(Matrix *image, const char *path){
  FILE *fp;

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
  image->pM=(unsigned char*)malloc(sizeof(unsigned char)*image->rows*image->cols);
  fread(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

  // Close file
  fclose(fp);
}

/**
 * @author: Joao Wellington and Messyo Sousa
 * @brief: Writes the Matrix Structure passed as input to this method in a
 *         specific path that was also passed as input to this method.
 * @param: Matrix *image, const char *path
 * @return: void
 */
void writeImage(Matrix *image, const char *path){
    FILE *fp;

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
 * @brief: This method performs all the calculus related to the Hough Transform.
 *         This occurs from the image scanning and calculation of the
 *         rho = xcos (theta) + ysin (theta) [0 < theta < 180] for pixels that
 *         display the high level (>THRESH_VALUE). It is worth mentioning that
 *         if the image passed as an input is not binarized, a threshold will
 *         be applied to it with a default threshold value set in THRESH_VALUE.
 * @param: Matrix *image, Matrix *accumulator, char out_type
 * @return: void
 */
void houghTransform(Matrix *image, Matrix *accumulator, char out_type){
  // Calculate the height and width of the Hough accumulator
  // accu_width  = 0 to 180 degrees
	// accu_height = 2*D - 1 and D = sqrt(height^2 + width^2)
	accumulator->rows        = 180;
  accumulator->cols        = ceil(2*(sqrt(image->rows*image->rows + image->cols*image->cols))) - 1;
  accumulator->grayscale   = image->grayscale;
	accumulator->pM          = (unsigned char*)calloc(accumulator->rows*accumulator->cols, sizeof(unsigned char));
  strcpy(accumulator->magicNumber,image->magicNumber);

  // Go to each pixel with hight level (>THRESH_VALUE) and calculate Rho to each Theta
	double rho;
	int theta,i,j;
	for(j=0; j<image->cols; j++){
		for(i=0; i<image->rows; i++){
			if(image->pM[ (j*image->rows) + i] > THRESH_VALUE){
				for(theta=0; theta<180; theta++){
					// rho = xcos(theta) + ysin(theta) [theta is in radians]
					rho = ( (j)*cos(theta*M_PI/180)  ) + ( (i)*sin(theta*M_PI/180) );

          if(out_type=='b'){
           // accumulator(theta,rho+D) = High Level
           accumulator->pM[ (int)((ceil(rho + accumulator->cols/2) * 180.0)) + 180-theta-1] = accumulator->grayscale;
          }
          else{
           // accumulator(theta,rho+D)++
           accumulator->pM[ (int)((ceil(rho + accumulator->cols/2) * 180.0)) + 180-theta-1]++;
          }
				}
			}
		}
	}
}
