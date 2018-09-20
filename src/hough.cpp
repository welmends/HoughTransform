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

int main() {
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
