/* colour squares: variant with numbers, non-rotatable */

#include <stdio.h>

#define SUM 10
#define N 16
#define X 5
#define Y 4

/* each block: upper left, upper right, lower left, lower right */
int blocks[N][4]={
	{2,4,5,8},{7,5,0,6},{2,1,2,2},{3,4,7,6},{6,3,8,3},{3,1,1,2},
	{1,4,5,3},{5,0,0,3},{4,4,8,2},{4,1,1,2},{2,3,7,4},{2,8,4,6},
	{5,6,2,4},{2,3,0,6},{8,3,4,3},{2,3,4,7}
};
int taken[N];

/* -1=blocked, 255=free */
int grid[X][Y]={
	{-1,255,255,-1},
	{255,255,255,255},
	{255,255,255,255},
	{255,255,255,255},
	{-1,255,255,-1}
};

/* check if sum at corner (x,y) is ok */
int ok(int x,int y) {
	int i,j,ix,sum=0,free=0,num=0,g;
	for(i=-1;i<1;i++) for(j=-1;j<1;j++) if(x+i>=0 && y+j>=0 && x+i<X && y+j<Y && grid[x+i][y+j]>=0) {
		num++; g=grid[x+i][y+j];
		if(g==255) free++;
		else { 
			ix=-i-j*2;
			sum+=blocks[g][ix];
		}
	}
	return num<=1 || (free && sum<=SUM) || (!free && sum==SUM);
}

void printgrid() {
	int i,j;
	for(j=0;j<Y;j++) {
		for(i=0;i<X;i++) {
			if(grid[i][j]<0) printf("** ");
			else if(grid[i][j]==255) printf("   ");
			else printf("%d%d ",blocks[grid[i][j]][0],blocks[grid[i][j]][1]);
		}
		putchar('\n');
		for(i=0;i<X;i++) {
			if(grid[i][j]<0) printf("** ");
			else if(grid[i][j]==255) printf("   ");
			else printf("%d%d ",blocks[grid[i][j]][2],blocks[grid[i][j]][3]);
		}
		puts("\n");
	}
}

int btr(int x,int y) {
	int i;
	if(x==X) {
		x=0,y++;
		if(y==Y) {
			printgrid();
			return 1;
		}
	}
	if(grid[x][y]<0) return btr(x+1,y);
	for(i=0;i<N;i++) if(!taken[i]) {
		grid[x][y]=i;
		taken[i]=1;
		if(!ok(x+1,y+1) || !ok(x,y+1) || !ok(x+1,y) || !ok(x,y)) goto fail;
		if(btr(x+1,y)) return 1;
	fail:
		taken[i]=0;
		grid[x][y]=255;
	}
	return 0;
}

int main() {
	int i;
	for(i=0;i<N;i++) taken[i]=0;
	btr(0,0);
	return 0;
}
