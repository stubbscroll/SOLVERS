/* lights off */

#include <stdio.h>

#define X 5
#define Y 5

int grid[Y][X]={
	{0,0,1,1,1},
	{0,0,1,1,1},
	{0,0,1,1,1},
	{0,0,1,1,1},
	{0,0,1,1,1}
};

int dx[]={0,1,0,-1,0};
int dy[]={0,0,1,0,-1};

int main() {
	int mask,i,j,k,x,y,d;
	int cur[Y][X];
	for(mask=0;mask<(1<<25);mask++) {
		for(i=0;i<Y;i++) for(j=0;j<X;j++) cur[i][j]=grid[i][j];
		for(k=i=0;i<Y;i++) for(j=0;j<X;j++,k++) if(mask&(1<<k)) {
			for(d=0;d<5;d++) {
				y=i+dx[d]; x=j+dy[d];
				if(x<0 || y<0 || x>=X || y>=Y) continue;
				cur[y][x]^=1;
			}
		}
		for(i=0;i<Y;i++) for(j=0;j<X;j++) if(cur[i][j]) goto fail;
		for(k=i=0;i<Y;i++) for(j=0;j<X;j++,k++) printf("%d%c",(mask&(1<<k))?1:0,j==X-1?'\n':' ');
		break;
	fail:;
	}
	return 0;
}
