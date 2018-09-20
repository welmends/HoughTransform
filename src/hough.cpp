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
void getLines(int thresh, int width, int height);

int main() {
  Mat input;
  Mat process;
  input = imread("lines.png", 0);
	Canny(input,process,100,150,3);

	int it;
	int width = process.cols;
	int height = process.rows;
	int thresh = width>height?width/4:height/4;

	//Transform
  houghTransform(process.data, width, height);

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
