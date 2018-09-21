/**
 * @file hough.c
 * @brief Hough Transform Implemantation
          Input : Path to an image, Type of the image (Canny or Original) and Output type (Binary or 8 Bits)
          Output: Hough Transform Matrix (Accumulator image [PGM])
 * @author $Author:$ de Souza, Joao Wellington Mendes; Brito, Messyo Sousa
 * @version $Revision:$
 * @date $Date:$ 21/09/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define IMAGE_PATH_SIZE (100)
#define THRESH_VALUE    (200)

typedef struct{
    int rows,cols,gray;
    unsigned char *pM;
}Matrix;

void instructions();
char* getImagePath(char **argv, char *out_type);
void readImage(Matrix *image, const char *path);
void writeImage(Matrix *image, const char *path);
void houghTransform(Matrix *image, Matrix *accumulator, char out_type);

/**
 *  Function: main
 *  Input: int argc, char **argv
 *  Output: int
 *  Purpose: The main method will call all other methods
 *  aiming to perform all operations prescribed. Also,
 *  the main method receives as input the arguments.
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
    return 0;
  }
}

/**
 *  Function: instructions
 *  Input: ---
 *  Output: void
 *  Purpose: Print on screen the instructions to use this
 *  program.
 */
void instructions(void){
  printf("\n");
  printf(" **************************************************** \n");
  printf("|                   [Instructions]                   |\n");
  //printf("|                 [Running the code]                 |\n");
  printf("| Command: ./hough.o <type> <out_type> <image_name>  |\n");
  printf("| Warning: The PGM image must be in 'images' folder  |\n");
  printf("| Example: ./hough.o canny binary lines              |\n");
  printf("| Example: ./hough.o original 8bits lines            |\n");
  printf("|                                                    |\n");
  printf("| <type>       : canny or original                   |\n");
  printf("| <out_type>   : binary or 8bits                     |\n");
  printf("| <image_name> : Any PGM image                       |\n");
  printf(" **************************************************** \n");
  printf("\n");
}

/**
 *  Function: getImagePath
 *  Input: char **argv, char *out_type
 *  Output: char*
 *  Purpose: Verifies the type arguments and returns a path to
 *  read a specifc image.
 */
char* getImagePath(char **argv, char *out_type){
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

  char *imagePath = (char*)calloc(IMAGE_PATH_SIZE, sizeof(char));
  strcpy(imagePath, "images/");
  strcat(imagePath, argv[1]);
  strcat(imagePath, "/");
  strcat(imagePath, argv[3]);
  strcat(imagePath, ".pgm");
  return imagePath;
}

/**
 *  Function: readImage
 *  Input: Matrix *image, const char *path
 *  Output: void
 *  Purpose: Since the path to a specific image was passed to
 *  this method, this function reads the header and the PGM
 *  image and stores this at the Matrix Structure passed as
 *  input to this method.
 */
void readImage(Matrix *image, const char *path){
    FILE *fp;
    fp = fopen(path, "r");
    char aux[3];
    int param1,param2,param3;

    fscanf(fp,"%s",aux);
    while(getc((fp)) == '#'){
      while(getc((fp)) != '\n');
    }
    fscanf(fp, "%d %d %d",&param1,&param2,&param3);

    image->rows=param1;
    image->cols=param2;
    image->gray=param3;
    image->pM=(unsigned char*)malloc(sizeof(unsigned char)*image->rows*image->cols);
    fread(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

    fclose(fp);
}

/**
 *  Function: writeImage
 *  Input: Matrix *image, const char *path
 *  Output: void
 *  Purpose: Writes the Matrix Structure passed as input to
 *  this method in a specific path that was also passed as
 *  input to this method.
 */
void writeImage(Matrix *image, const char *path){
    FILE *fp;
    fp = fopen(path, "w");

    fprintf(fp, "P5\n");
    fprintf(fp, "# PGM file\n");
    fprintf(fp, "%d %d\n",image->rows, image->cols);
    fprintf(fp, "%d\n",image->gray);
    fwrite(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

    fclose(fp);
}

/**
 *  Function: houghTransform
 *  Input: Matrix *image, Matrix *accumulator, char out_type
 *  Output: void
 *  Purpose: This function performs all the calculus related
 *  to the Hough Transform. This occurs from the image scanning
 *  and calculation of the rho = xcos (theta) + ysin (theta)
 *  [0 < theta <180] for pixels that display the high level.
 *  It is worth mentioning that if the image passed as an
 *  input is not binarized, a threshold will be applied to
 *  it with a default threshold value set in THRESH_VALUE.
 */
void houghTransform(Matrix *image, Matrix *accumulator, char out_type){
	//accu_height = 2*D and D = sqrt(height^2 + width^2)
	accumulator->cols  = ceil(2*(sqrt(image->rows*image->rows + image->cols*image->cols))) - 1;
	accumulator->rows  = 180;
  accumulator->gray  = 255;
	accumulator->pM    = (unsigned char*)calloc(accumulator->rows*accumulator->cols, sizeof(unsigned char));

	double rho;
	int theta,i,j;
	for(j=0; j<image->cols; j++){
		for(i=0; i<image->rows; i++){
			if(image->pM[ (j*image->rows) + i] > THRESH_VALUE){
				for(theta=0; theta<180; theta++){
					// rho = xcos(theta) + ysin(theta)
					rho = ( (j)*cos(theta*M_PI/180)  ) + ( (i)*sin(theta*M_PI/180) );
					// accumulator(theta,rho+D)++
          if((int)((ceil(rho + accumulator->cols/2) * 180.0)) + theta>0 &&
             (int)((ceil(rho + accumulator->cols/2) * 180.0)) + theta<accumulator->rows*accumulator->cols){
               if(out_type=='b'){
                 accumulator->pM[ (int)((ceil(rho + accumulator->cols/2) * 180.0)) + 180-theta-1]=255;
               }
               else{
                 accumulator->pM[ (int)((ceil(rho + accumulator->cols/2) * 180.0)) + 180-theta-1]++;
               }
          }
				}
			}
		}
	}
}
