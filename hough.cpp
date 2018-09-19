//gcc hough.c -o hough.o `pkg-config --cflags --libs opencv` -lstdc++ -lm
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

typedef struct{
	int x1;
	int y1;
	int x2;
	int y2;
}Line;

FILE *file;
Matrix *image;
Line* lines;
int lines_size;
unsigned int* accumulator;
int accumulator_width;
int accumulator_height;

void houghTransform(unsigned char* img_data, int width, int height);
void getLines(int thresh, int width, int height);

void readImageHeader(const char *path);
void readImage(const char *path);
void saveAccumulatorToFile(const char *path);
void saveLinesToFile(const char *path);

int main() {
  image=(Matrix*)calloc(1,sizeof(Matrix));
	readImageHeader("line.pgm");
  readImage("line.pgm");

  printf("%d, %d\n",image->cols, image->rows);
  // Mat input = Mat::zeros(image->cols, image->rows, CV_8UC1);
	// for(int i=0; i<image->cols; i++){
  //   for(int j=0; j<image->rows; j++){
  //     input.at<uchar>(j,i) = image->pM[j*image->cols + i];
  //   }
  // }
  //
	// Mat process;
	// Canny(input,process,100,150,3);
  //
	// int it;
	// int width = process.cols;
	// int height = process.rows;
	// int thresh = width>height?width/4:height/4;
  //
  // imshow("image", process);
  // waitKey();
  //
	// //Transform
	// houghTransform(process.data, width, height);
  //
	// //Search the accumulator
	// getLines(thresh, width, height);
//
// 	//Draw the results
// 	for(it=0; it<lines_size; it++){
// 		line(process, Point(lines[it].x1, lines[it].y1), Point(lines[it].x2, lines[it].y2), Scalar::all(128), 2, 8);
// 	}
// 	saveLinesToFile("lines.txt");
//
// 	//Visualize all
// 	int aw, ah, maxa;
// 	aw = ah = maxa = 0;
// 	aw = accumulator_width;
// 	ah = accumulator_height;
//
// 	for(int p=0;p<(ah*aw);p++){
// 		if((int)accumulator[p] > maxa)
// 			maxa = accumulator[p];
// 	}
// 	double contrast = 1.0;
// 	double coef = 255.0 / (double)maxa * contrast;
//
// 	Mat accu(ah, aw, CV_8UC3);
// 	for(int p=0;p<(ah*aw);p++){
// 		unsigned char c = (double)accumulator[p] * coef < 255.0 ? (double)accumulator[p] * coef : 255.0;
// 		accu.data[(p*3)+0] = 255;
// 		accu.data[(p*3)+1] = 255-c;
// 		accu.data[(p*3)+2] = 255-c;
// 	}
//
// 	cv::imshow("CW_IMG_ORIGINAL", process);
// 	cv::imshow("CW_ACCUMULATOR", accu);
// 	cv::waitKey();
//
 	return 0;
}

void houghTransform(unsigned char* img_data, int width, int height){
	//Create the accumulator
	accumulator_height = ((sqrt(2.0) * (height>width?height:width)) / 2.0) * 2.0; // -r -> +r
	accumulator_width = 180;
	accumulator = (unsigned int*)calloc(accumulator_height*accumulator_width, sizeof(unsigned int));

	double rho;
	int theta,i,j;
	for(j=0; j<height; j++){
		for(i=0; i<width; i++){
			if(img_data[ (j*width) + i] == 255){
				for(theta=0; theta<180; theta++){
					// rho = xcos(theta) + ysin(theta)
					rho = ( (i-(width/2))*cos(theta*M_PI/180)  ) +
					      ( (j-(height/2))*sin(theta*M_PI/180) );
					// accumulator(theta,rho)++
					accumulator[ (int)((round(rho + accumulator_height/2) * 180.0)) + theta]++;
				}
			}
		}
	}
}

void getLines(int thresh, int width, int height){
	lines = (Line*)calloc(1, sizeof(Line));
	lines_size = 0;
	Line l;

	if(accumulator == 0)
		return;

	for(int r=0;r<accumulator_height;r++){
		for(int t=0;t<accumulator_width;t++){
			if((int)accumulator[(r*accumulator_width) + t] >= thresh){
				//Is this point a local maxima (9x9)
				int max = accumulator[(r*accumulator_width) + t];
				for(int ly=-4;ly<=4;ly++){
					for(int lx=-4;lx<=4;lx++){
						if( (ly+r>=0 && ly+r<accumulator_height) && (lx+t>=0 && lx+t<accumulator_width)  ){
							if( (int)accumulator[( (r+ly)*accumulator_width) + (t+lx)] > max ){
								max = accumulator[( (r+ly)*accumulator_width) + (t+lx)];
								ly = lx = 5;
							}
						}
					}
				}
				if(max > (int)accumulator[(r*accumulator_width) + t])
					continue;

				l.x1 = l.y1 = l.x2 = l.y2 = 0;

				if(t >= 45 && t <= 135){
					//y = (r - x cos(t)) / sin(t)
					l.x1 = 0;
					l.y1 = ((double)(r-(accumulator_height/2)) - ((l.x1 - (width/2) ) * cos(t * M_PI/180))) / sin(t * M_PI/180) + (height / 2);
					l.x2 = width - 0;
					l.y2 = ((double)(r-(accumulator_height/2)) - ((l.x2 - (width/2) ) * cos(t * M_PI/180))) / sin(t * M_PI/180) + (height / 2);
				}
				else{
					//x = (r - y sin(t)) / cos(t);
					l.y1 = 0;
					l.x1 = ((double)(r-(accumulator_height/2)) - ((l.y1 - (height/2) ) * sin(t * M_PI/180))) / cos(t * M_PI/180) + (width / 2);
					l.y2 = height - 0;
					l.x2 = ((double)(r-(accumulator_height/2)) - ((l.y2 - (height/2) ) * sin(t * M_PI/180))) / cos(t * M_PI/180) + (width / 2);
				}
				lines_size++;
        printf("%d\n",lines_size);
				//lines = (Line*)realloc(lines, lines_size);
				//lines[lines_size-1] = l;
			}
		}
	}

	printf("lines: %d | thresh: %d\n", (int)lines_size, thresh);
}

void readImageHeader(const char *path){
    /* Ler o cabeçalho da imagem */
    FILE *fp;

    fp = fopen(path, "r");
    char aux[3];
    char aux2[1];
    int param1;
    int param2;
    int param3;

    fscanf(fp,"%s",aux);
    fscanf(fp,"%c",aux2);
    while(true){
      fscanf(fp,"%c",aux2);
      if(*aux2=='\n')
        break;
    }
    fscanf(fp, "%d %d %d",&param1,&param2,&param3);

    image->rows=param1;
    image->cols=param2;
    image->gray=param3;

    fclose(fp);
}

void readImage(const char *path){
    /* Ler a imagem */

    FILE *fp;

    fp = fopen(path, "rb");

    fseek(fp, 15, SEEK_SET);//Reposicionando o local de leitura do arquivo

    image->pM=(unsigned char*)malloc(sizeof(unsigned char)*image->rows*image->cols);

    fread(image->pM, sizeof(unsigned char), image->rows*image->cols, fp);

    fclose(fp);
}

void saveAccumulatorToFile(const char *path){
	return;
}

void saveLinesToFile(const char *path){
	file = fopen(path, "w");
	int it;
	for(it=0; it<lines_size; it++){
		fprintf(file,"(%d,%d) (%d,%d)\n",lines[it].x1,lines[it].y1,lines[it].x2,lines[it].y2);
	}
	fclose(file);
}
