/* click cells in image and toggle neighbouring cells between red, green, blue
   for "oak grove" puzzle, correct is green */

#include <stdio.h>

#define X 4
#define Y 3

/* start state: red=0, green=1, blue=2 */
int start[Y][X]={{0,0,0,0},{0,0,0,0},{0,0,0,0}};
int goal[Y][X]={{1,1,1,1},{1,1,1,1},{1,1,1,1}};
/* flip[i][j]: cells that are flipped when pressing (i,j) */
int flip[Y][X][Y][X]={
	{{{1,0,0,0},{1,0,0,0},{1,1,0,0}},
	{{0,1,0,0},{0,1,0,0},{1,1,0,0}},
	{{0,0,1,0},{0,0,1,0},{0,0,1,1}},
	{{0,0,0,1},{0,0,0,1},{0,0,1,1}}},
	{{{0,0,1,0},{1,1,1,0},{0,0,0,0}},
	{{0,0,0,1},{0,1,1,1},{0,0,0,0}},
	{{1,0,0,0},{1,1,1,0},{0,0,0,0}},
	{{0,1,1,1},{0,0,0,1},{0,0,0,0}}},
	{{{0,0,0,0},{0,0,1,0},{1,1,1,0}},
	{{0,1,1,0},{0,1,0,0},{0,1,0,0}},
	{{0,0,1,1},{0,0,1,0},{0,0,1,0}},
	{{0,0,0,0},{0,1,1,1},{0,0,0,1}}}};

int main() {
	int i,j,try,this,k,flips,l;
	int cur[Y][X];
	for(try=0;try<3*3*3*3*3*3*3*3*3*3*3*3;try++) {
		this=try;
		for(i=0;i<Y;i++) for(j=0;j<X;j++) cur[i][j]=start[i][j];
		for(k=0;k<Y;k++) for(l=0;l<X;l++) {
			flips=this%3; this/=3;
			while(flips--) for(i=0;i<Y;i++) for(j=0;j<X;j++) cur[i][j]=(cur[i][j]+flip[k][l][i][j])%3;
		}
		for(i=0;i<Y;i++) for(j=0;j<X;j++) if(cur[i][j]!=goal[i][j]) goto fail;
		for(i=0;i<Y;i++) for(j=0;j<X;j++,try/=3) printf("%d%c",try%3,j==X-1?'\n':' ');
		break;
	fail:;
	}
	return 0;
}
