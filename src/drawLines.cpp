//  USE IT
//
//	//Search the accumulator
//	getLines(thresh, width, height);
//
// 	//Draw the results
// 	for(it=0; it<lines_size; it++){
// 		line(process, Point(lines[it].x1, lines[it].y1), Point(lines[it].x2, lines[it].y2), Scalar::all(128), 2, 8);
// 	}
// 	saveLinesToFile("lines.txt");

typedef struct{
	int x1;
	int y1;
	int x2;
	int y2;
}Line;


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

void saveLinesToFile(const char *path){
	file = fopen(path, "w");
	int it;
	for(it=0; it<lines_size; it++){
		fprintf(file,"(%d,%d) (%d,%d)\n",lines[it].x1,lines[it].y1,lines[it].x2,lines[it].y2);
	}
	fclose(file);
}
