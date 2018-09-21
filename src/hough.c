#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define IMAGE_PATH_SIZE 100

typedef struct{
    int rows,cols,gray;
    unsigned char *pM;
}Matrix;

void instructions();
char* getImagePath(char **argv, char *out_type);
void readImage(Matrix *image, const char *path);
void writeImage(Matrix *image, const char *path);
void houghTransform(Matrix *image, Matrix *accumulator, char out_type);

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

void instructions(){
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
void readImage(Matrix *image, const char *path){
    FILE *fp;

    fp = fopen(path, "r");
    char aux[3];
    int param1;
    int param2;
    int param3;

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
			if(image->pM[ (j*image->rows) + i] == 255){
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
