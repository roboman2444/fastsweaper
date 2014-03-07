#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1


int cursorx, cursory;
int gx, gy, minecount;
char * grid;
char * shown;
char * done;
int numtest = 1;

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

int fillRandom(int count){
	if(!grid) return FALSE;
	if(count>gx*gy) return FALSE;
	srand(time(NULL));
	for(; count > 0; count--){
		int xpos = ((float)rand() / (float)RAND_MAX) * (float)gx;
		int ypos = ((float)rand() / (float)RAND_MAX) * (float)gy;
		int gridpos = (xpos + ypos*gx);
		if(grid[gridpos] == 10){
			count++;
			continue;
		}
		grid[gridpos] = 10;
	}
	return TRUE;
}
int printGrid(char * mygrid){
	if(!mygrid) return FALSE;
	int xc, yc;
	for(yc = 0; yc < gy; yc++){
		for(xc = 0; xc < gx; xc++){
			int gridpos = (xc+ yc*gx);
			if(mygrid[gridpos] == 10)	printf("\x1b[32m\x1b[41m*");
			else if(mygrid[gridpos] == 0)	printf(" ");
			else if(mygrid[gridpos] == 11)	printf("-");
			else if(mygrid[gridpos] == 12)	printf("\x1b[41mM");
			else printf("\x1b[3%im%i", mygrid[gridpos], mygrid[gridpos]);
		}
		printf("\n");
	}
	return TRUE;
}
int printGridC(int cx, int cy, char * mygrid){
	if(!mygrid) return FALSE;
	int xc, yc;
	printf("\033[%iA",gy);
	for(yc = 0; yc < gy; yc++){
		for(xc = 0; xc < gx; xc++){
			if(xc ==cx && yc ==cy)printf("\x1b[47m");
			int gridpos = (xc+yc*gx);
			if(mygrid[gridpos] == 10)	printf("\x1b[32m\x1b[41m*");
			else if(mygrid[gridpos] == 0)	printf(" ");
			else if(mygrid[gridpos] == 11)	printf("-");
			else if(mygrid[gridpos] == 12)	printf("\x1b[41mM");
			else printf("\x1b[3%im%i", mygrid[gridpos], mygrid[gridpos]);
			printf("\x1b[0m");
		}
		printf("\n");
	}
	return TRUE;
}
void expandGrid(int x, int y){
		if(x >= gx) return;
		if(y >= gy) return;
		if(x < 0) return;
		if(y < 0) return;
		if(!grid) return;
		if(!shown) return;
		int gridpos = (x+y*gx);
		if(shown[gridpos]!=11) return; //return if this grid has already been shown

		if(grid[gridpos] > 0 && grid[gridpos] < 10){
			shown[gridpos] = grid[gridpos];
//			expandGrid(x+1, y, maxx, maxy, showngrid, testgrid);
//			expandGrid(x-1, y, maxx, maxy, showngrid, testgrid);
//			expandGrid(x, y-1, maxx, maxy, showngrid, testgrid);
//			expandGrid(x, y+1, maxx, maxy, showngrid, testgrid);
		} else if(grid[gridpos] != 10){
			shown[gridpos] = 0;
			expandGrid(x+1, y);
			expandGrid(x-1, y);
			expandGrid(x, y-1);
			expandGrid(x, y+1);
		} else {
			//should never happen, but whatevs
		}

}
int testPoint(int x, int y){
		if(x >= gx) return FALSE;
		if(y >= gy) return FALSE;
		if(x < 0) return FALSE;
		if(y < 0) return FALSE;
		if(!grid) return FALSE;
		int gridpos = (x+ y*gx);
		if(grid[gridpos] == 10) return TRUE;
		return FALSE;
}


int createNumTable(){
	if(!grid) return FALSE;
	int xc, yc;
	for(yc = 0; yc < gy; yc++){
		for(xc = 0; xc < gx; xc++){
			int c = 0;
			int gridpos = (xc + yc * gx);
			if(grid[gridpos] == 10) continue;

			c += testPoint(xc-1, yc-1);
			c += testPoint(xc, yc-1);
			c += testPoint(xc+1, yc-1);

			c += testPoint(xc-1, yc);
			c += testPoint(xc+1, yc);

			c += testPoint(xc-1, yc+1);
			c += testPoint(xc, yc+1);
			c += testPoint(xc+1, yc+1);
			grid[gridpos] = c;
		}
	}
	return TRUE;
}

int moveCursor(char dir){
	switch(dir){
		case 's':
			if(cursory < gy-1) cursory++;
			else cursory = 0;
			break;
		case 'd':
			if(cursorx < gx-1) cursorx++;
			else cursorx = 0;
			break;
		case 'w':
			if(cursory > 0) cursory--;
			else cursory = gy-1;
			break;
		case 'a':
			if(cursorx > 0) cursorx--;
			else cursorx = gx-1;
			break;
		default: break;
	}
	return TRUE;
}

char checkFlagged(int x, int y){
	if(x >= gx) return FALSE;
	if(y >= gy) return FALSE;
	if(x < 0) return FALSE;
	if(y < 0) return FALSE;
	int gridpos = (x+y*gx);
	if(shown[gridpos] == 12) return TRUE;
	return FALSE;
}

char countFlagged(int x, int y){
	if(x >= gx) return FALSE;
	if(y >= gy) return FALSE;
	if(x < 0) return FALSE;
	if(y < 0) return FALSE;

	char c;
	c  = checkFlagged(x-1, y-1);
	c += checkFlagged(x, y-1);
	c += checkFlagged(x+1, y-1);

	c += checkFlagged(x-1, y);
	c += checkFlagged(x+1, y);

	c += checkFlagged(x-1, y+1);
	c += checkFlagged(x, y+1);
	c += checkFlagged(x+1, y+1);
	return c;
}
char checkUntested(int x, int y){
	if(x >= gx) return FALSE;
	if(y >= gy) return FALSE;
	if(x < 0) return FALSE;
	if(y < 0) return FALSE;
	int gridpos = (x+y*gx);
	if(shown[gridpos] == 11) return TRUE;
	return FALSE;
}

char countUntested(int x, int y){
	if(x >= gx) return FALSE;
	if(y >= gy) return FALSE;
	if(x < 0) return FALSE;
	if(y < 0) return FALSE;

	char c;
	c  = checkUntested(x-1, y-1);
	c += checkUntested(x, y-1);
	c += checkUntested(x+1, y-1);

	c += checkUntested(x-1, y);
	c += checkUntested(x+1, y);

	c += checkUntested(x-1, y+1);
	c += checkUntested(x, y+1);
	c += checkUntested(x+1, y+1);
	return c;
}

int solve(void){

	int x,y;
	for(x = 0; x < gx; x++){
		for(y = 0; y < gy; y++){
			int gridpos = (x+y*gx);
			int num = shown[gridpos];
			if(num > 7) continue;
			if(done[gridpos]) continue;
			int flagged = countFlagged(x,y);

			//can get rid of the rest
			//todo make this into countUntested for recursive
			if(num == flagged){
				done[gridpos] = TRUE;
				numtest++;
				expandGrid(x-1, y-1);
				expandGrid(x, y-1);
				expandGrid(x+1, y-1);

				expandGrid(x-1, y);
				expandGrid(x+1, y);

				expandGrid(x-1, y+1);
				expandGrid(x, y+1);
				expandGrid(x+1, y+1);
//				return TRUE;
			}
			else if(num == countUntested(x, y) + flagged){
				done[gridpos] = TRUE;
				numtest++;
				int nos;
				int tx = gx-1;
				if(y>0){
					nos = ((x-1) + (y-1)*gx);
					if(shown[nos] == 11) shown[nos] = 12;
					nos++;
					if(shown[nos] == 11) shown[nos] = 12;
					if(x<tx){
						nos++;
						if(shown[nos] == 11) shown[nos] = 12;
					}
				}
				nos = (x-1 + y*gx);
				if(shown[nos] == 11) shown[nos] = 12;
				if(x<tx){
					nos+=2;
					if(shown[nos] == 11) shown[nos] = 12;
				}
				if(y<gy-1){
					nos = ((x-1) + (y+1)*gx);
					if(shown[nos] == 11) shown[nos] = 12;
					nos++;
					if(shown[nos] == 11) shown[nos] = 12;
					if(x<tx){
						nos++;
						if(shown[nos] == 11) shown[nos] = 12;
					}
				}
//				return TRUE;
			}

		}
	}
	return FALSE;
}


int main(int argc, char * argv[]){
	if(argc < 4){
		gx = 40;
		gy = 40;
		minecount = 200;
	} else {
		gx = atoi(argv[1]);
		gy = atoi(argv[2]);
		minecount = atoi(argv[3]);
	}
	cursorx = gx/2;
	cursory = gy/2;
	//todo
	grid = malloc(gx*gy*sizeof(char));
	shown = malloc(gx*gy*sizeof(char));
	done = malloc(gx*gy*sizeof(char));
	memset(done, 0, gx*gy*sizeof(char));
	memset(shown, 11, gx*gy*sizeof(char));
	memset(grid, 0, gx*gy*sizeof(char));
	fillRandom(minecount);
	createNumTable(grid);
	printGrid(shown);
	printGridC(cursorx, cursory, shown);


	char c;
	while(c != 'x' && (c=mygetch()) != 'x'){
		switch(c){
			case 'w': moveCursor('w'); break;
			case 'a': moveCursor('a'); break;
			case 's': moveCursor('s'); break;
			case 'd': moveCursor('d'); break;
			case 'e': solve(); break;
			case 'q': {int z; for(z=0; z<10; z++) solve(); }break;
			case 'm': {
				int gridpos = (cursorx+ cursory*gx);
				if(shown[gridpos] == 11){
					shown[gridpos] = 12;
				} else if(shown[gridpos] == 12){
					shown[gridpos] = 11;
				}

			} break;
			case ' ': {
				if(testPoint(cursorx, cursory)){
					c = 'x';
					free(shown);
					shown = grid;
				} else {
					expandGrid(cursorx, cursory);
				}
				} break;
			default: break;
		}
			printGridC(cursorx, cursory, shown);

	}
	if(shown) free(shown);
	free(done);
	if(grid == shown) return FALSE;
	free(grid);
	return FALSE;
}
