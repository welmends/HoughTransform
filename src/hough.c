#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
    int rows,cols,gray;
    unsigned char *pM;
}Matrix;

void houghTransform(Matrix *image, Matrix *accumulator);
void readImage(Matrix *image, const char *path);
void writeImage(Matrix *image, const char *path);

int main() {
  Matrix *image=(Matrix*)calloc(1,sizeof(Matrix));
  Matrix *accumulator = (Matrix*)calloc(1,sizeof(Matrix));

  readImage(image, "images/canny/line.pgm");

  houghTransform(image, accumulator);

  writeImage(accumulator, "/home/well/Desktop/accu.pgm");

 	return 0;
}

void houghTransform(Matrix *image, Matrix *accumulator){
	//accu_height = 2*D and D = sqrt(height^2 + width^2)
	accumulator->rows  = ceil(2*(sqrt(image->rows*image->rows + image->cols*image->cols)));
	accumulator->cols  = 180;
  accumulator->gray  = 255;
	accumulator->pM    = (unsigned char*)calloc(accumulator->rows*accumulator->cols, sizeof(unsigned char));

	double rho;
	int theta,i,j;
	for(j=0; j<image->rows; j++){
		for(i=0; i<image->cols; i++){
			if(image->pM[ (j*image->cols) + i] == 255){
				for(theta=0; theta<180; theta++){
					// rho = xcos(theta) + ysin(theta)
					rho = ( (i)*cos(theta*M_PI/180)  ) + ( (j)*sin(theta*M_PI/180) );
					// accumulator(theta,rho+D)++
          if((int)((ceil(rho + accumulator->rows/2) * 180.0)) + theta>0 &&
             (int)((ceil(rho + accumulator->rows/2) * 180.0)) + theta<accumulator->rows*accumulator->cols){
               	accumulator->pM[ (int)((ceil(rho + accumulator->rows/2) * 180.0)) + theta]++;
          }
				}
			}
		}
	}
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
