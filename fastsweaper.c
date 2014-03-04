#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1


int cursorx, cursory;
int gx, gy;

int mygetch ( void )
{
  int ch;
  struct termios oldt, newt;

  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}

int fillRandom(int x, int y, int count, char * grid){
	if(!grid) return FALSE;
	if(count>x*y) return FALSE;
	srand(time(NULL));
	for(; count > 0; count--){
		int xpos = ((float)rand() / (float)RAND_MAX) * (float)x;
		int ypos = ((float)rand() / (float)RAND_MAX) * (float)y;
		int gridpos = (xpos + ypos*x);
		if(grid[gridpos] == 10){
			count++;
			continue;
		}
		grid[gridpos] = 10;
	}
	return TRUE;
}
int printGrid(int x, int y, char * grid){
	if(!grid) return FALSE;
	int xc, yc;
	for(yc = 0; yc < y; yc++){
		for(xc = 0; xc < x; xc++){
			int gridpos = (xc+ yc*x);
			if(grid[gridpos] == 10)	printf("\x1b[32m\x1b[41m*\x1b[0m");
			else if(grid[gridpos] == 0)	printf("-");
			else if(grid[gridpos] == 11)	printf(" ");
			else if(grid[gridpos] == 12)	printf("\x1b[41mM\x1b[0m");
			else printf("\x1b[3%im%i\x1b[0m", grid[gridpos], grid[gridpos]);
		}
		printf("\n");
	}
	return TRUE;
}
int printGridC(int x, int y, int cx, int cy, char * grid){
	if(!grid) return FALSE;
	int xc, yc;
	printf("\033[%iA",y);
	for(yc = 0; yc < y; yc++){
		for(xc = 0; xc < x; xc++){
			if(xc ==cx && yc ==cy)printf("\x1b[47m");
			int gridpos = (xc+ yc*x);
			if(grid[gridpos] == 10)	printf("\x1b[32m\x1b[41m*\x1b[0m");
			else if(grid[gridpos] == 0)	printf("-");
			else if(grid[gridpos] == 11)	printf(" ");
			else if(grid[gridpos] == 12)	printf("\x1b[41mM\x1b[0m");
			else printf("\x1b[3%im%i\x1b[0m", grid[gridpos], grid[gridpos]);
			printf("\x1b[0m");
		}
		printf("\n");
	}
	return TRUE;
}
void expandGrid(int x, int y, int maxx, int maxy, char * showngrid, char * testgrid){
		if(x >= maxx) return;
		if(y >= maxy) return;
		if(x < 0) return;
		if(y < 0) return;
		if(!testgrid) return;
		if(!showngrid) return;
		int gridpos = (x+ y*maxx);
		if(showngrid[gridpos]) return; //return if this grid has already been shown

		if(testgrid[gridpos] > 0 && testgrid[gridpos] < 10){
			showngrid[gridpos] = testgrid[gridpos];
//			expandGrid(x+1, y, maxx, maxy, showngrid, testgrid);
//			expandGrid(x-1, y, maxx, maxy, showngrid, testgrid);
//			expandGrid(x, y-1, maxx, maxy, showngrid, testgrid);
//			expandGrid(x, y+1, maxx, maxy, showngrid, testgrid);
		} else if(testgrid[gridpos] != 10){
			showngrid[gridpos] = 11;
			expandGrid(x+1, y, maxx, maxy, showngrid, testgrid);
			expandGrid(x-1, y, maxx, maxy, showngrid, testgrid);
			expandGrid(x, y-1, maxx, maxy, showngrid, testgrid);
			expandGrid(x, y+1, maxx, maxy, showngrid, testgrid);
		} else {
			//should never happen, but whatevs
		}

}
int testPoint(int x, int y, int maxx, int maxy, char * grid){
		if(x >= maxx) return FALSE;
		if(y >= maxy) return FALSE;
		if(x < 0) return FALSE;
		if(y < 0) return FALSE;
		if(!grid) return FALSE;
		int gridpos = (x+ y*maxx);
		if(grid[gridpos] == 10) return TRUE;
		return FALSE;
}


int createNumTable(int x, int y, char * grid){
	if(!grid) return FALSE;
	int xc, yc;
	for(yc = 0; yc < y; yc++){
		for(xc = 0; xc < x; xc++){
			int c = 0;
			int gridpos = (xc + yc * x);
			if(grid[gridpos] == 10) continue;

			int testnegx = xc - 1;
			int testnegy = yc - 1;
			int testposx = xc + 1;
			int testposy = yc + 1;

			c += testPoint(testnegx, testnegy, x, y, grid);
			c += testPoint(xc, testnegy,x, y, grid);
			c += testPoint(testposx, testnegy,x, y, grid);

			c += testPoint(testnegx, yc,x, y, grid);
			c += testPoint(testposx, yc,x, y, grid);

			c += testPoint(testnegx, testposy,x, y, grid);
			c += testPoint(xc, testposy,x, y, grid);
			c += testPoint(testposx, testposy,x, y, grid);
			grid[gridpos] = c;
		}
	}
	return TRUE;
}

int moveCursor(char dir){
	switch(dir){
		case 's':
			if(cursory < gy-1) cursory++;
			break;
		case 'd':
			if(cursorx < gx-1) cursorx++;
			break;
		case 'w':
			if(cursory > 0) cursory--;
			break;
		case 'a':
			if(cursorx > 0) cursorx--;
			break;
		default: break;
	}
	return TRUE;
}

int main(int argc, char * argv[]){
	int xsize, ysize;
	char * grid;
	char * shown;
	if(argc < 3){
		xsize = 20;
		ysize = 20;
	}
	gy = ysize;
	gx = xsize;
	cursorx = xsize/2;
	cursory = ysize/2;
	//todo
	grid = malloc(xsize*ysize*sizeof(char));
	shown = malloc(xsize*ysize*sizeof(char));
	memset(shown, 0, xsize*ysize*sizeof(char));
	memset(grid, 0, xsize*ysize*sizeof(char));
	fillRandom(xsize, ysize, 50, grid);
//	printGrid(xsize, ysize, grid);
	createNumTable(xsize, ysize, grid);
//	printf("\n");
//	printGrid(xsize, ysize, grid);
//	printf("\n");
	printGrid(xsize, ysize, shown);
//	printf("\n");
//	expandGrid(5, 5, xsize, ysize, shown, grid);
//	printGrid(xsize, ysize, shown);
	printGridC(xsize, ysize, cursorx, cursory, shown);


	char c;
	while(c != 'x' && (c=mygetch()) != 'x'){
		switch(c){
			case 'w': moveCursor('w'); break;
			case 'a': moveCursor('a'); break;
			case 's': moveCursor('s'); break;
			case 'd': moveCursor('d'); break;
			case 'm': {
				int gridpos = (cursorx+ cursory*xsize);
				if(shown[gridpos] == 0){
					shown[gridpos] = 12;
				} else if(shown[gridpos] == 12){
					shown[gridpos] = 0;
				}

			} break;
			case ' ': {
				if(testPoint(cursorx, cursory, xsize, ysize, grid)){
					c = 'x';
					free(shown);
					shown = grid;
				} else {
					expandGrid(cursorx, cursory, xsize, ysize, shown, grid);
				}
				} break;
			default: break;
		}
			printGridC(xsize, ysize, cursorx, cursory, shown);

	}
	return FALSE;
}
