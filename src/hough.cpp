#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

typedef struct{
    int rows,cols;
    int gray;
    unsigned char *pM;

}Matrix;
Matrix *image;

unsigned int* accumulator;
int accumulator_width;
int accumulator_height;

void houghTransform(unsigned char* img_data, int width, int height);
void readImage(const char *path);
void writeImage(const char *path);

int main() {
  image=(Matrix*)calloc(1,sizeof(Matrix));
  readImage("images/bug.PGM");

  cout<<image->rows<<endl;
  cout<<image->cols<<endl;
  Mat input = Mat::zeros(image->cols, image->rows, CV_8UC1);
  for(int i=0; i<image->cols; i++){
    for(int j=0; j<image->rows; j++){
      input.at<uchar>(i,j) = image->pM[i*image->rows + j];
    }
  }

  imshow("image", input);
  waitKey();
  writeImage("/home/well/Desktop/cu.PGM");
  /*
  Mat input;
  Mat process;
  input = imread("images/lines.png", 0);
	Canny(input,process,100,150,3);

	int width = process.cols;
	int height = process.rows;

	//Transform
  houghTransform(process.data, width, height);

  cout<<accumulator_width<<endl;
  cout<<accumulator_height<<endl;

  Mat H = Mat::zeros(accumulator_height, accumulator_width, CV_8UC1);
  for(int i=0; i<H.rows; i++){
    for(int j=0; j<H.cols; j++){
      if(accumulator[i*H.cols + j]<1){
          H.at<uchar>(i,j) = 0;
      }else{
        H.at<uchar>(i,j) = 255;
      }
      H.at<uchar>(i,j) = 50+accumulator[i*H.cols + j];
    }
  }
  resize(H,H,Size(550,390));
  imshow("H", H);
  waitKey();
  */
 	return 0;
}

void houghTransform(unsigned char* img_data, int width, int height){
	//accu_height = 2*D and D = sqrt(height^2 + width^2)
	accumulator_height = ceil(2*(sqrt(height*height + width*width)));
	accumulator_width  = 180;
	accumulator        = (unsigned int*)calloc(accumulator_height*accumulator_width, sizeof(unsigned int));

	double rho;
	int theta,i,j;
	for(j=0; j<height; j++){
		for(i=0; i<width; i++){
			if(img_data[ (j*width) + i] == 255){
				for(theta=0; theta<180; theta++){
					// rho = xcos(theta) + ysin(theta)
					rho = ( (i)*cos(theta*M_PI/180)  ) + ( (j)*sin(theta*M_PI/180) );
					// accumulator(theta,rho+D)++
          if((int)((ceil(rho + accumulator_height/2) * 180.0)) + theta>0 &&
             (int)((ceil(rho + accumulator_height/2) * 180.0)) + theta<accumulator_height*accumulator_width){
               	accumulator[ (int)((ceil(rho + accumulator_height/2) * 180.0)) + theta]++;
          }
				}
			}
		}
	}
}
void readImage(const char *path){
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
void writeImage(const char *path){
    FILE *fp;

    fp = fopen(path, "w");

    fprintf(fp, "P5\n");
    fprintf(fp, "# PGM file\n");
    fprintf(fp, "%d %d\n",image->rows, image->cols);
    fprintf(fp, "%d\n",image->gray);
    fwrite(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

    fclose(fp);
}
