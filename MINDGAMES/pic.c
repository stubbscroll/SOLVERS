/* click cells in image and toggle neighbouring cells between red, green, blue
   for "oak grove" puzzle, correct is green */

#include <stdio.h>

/* start state: red=0, green=1, blue=2 */
/* flip[i][j]: cells that are flipped when pressing (i,j) */

/* start picture, goal is green */

#define X 3
#define Y 3

int start[Y][X]={{0,0,0},{0,0,1},{0,0,0}};
int goal[Y][X]={{1,1,1},{1,1,1},{1,1,1}};
int flip[Y][X][Y][X]={
	{{{1,1,0},{1,0,0},{0,0,0}},
	{{1,1,1},{0,0,0},{0,0,0}},
	{{0,1,1},{0,0,1},{0,0,0}}},
	{{{1,0,0},{1,0,0},{1,0,0}},
	{{0,1,0},{1,1,1},{0,1,0}},
	{{0,0,1},{0,0,1},{0,0,1}}},
	{{{0,0,0},{1,0,0},{1,1,0}},
	{{0,0,0},{0,0,0},{1,1,1}},
	{{0,0,0},{0,0,1},{0,1,1}}}};

int main() {
	int i,j,try,this,k,flips,l;
	int cur[Y][X];
	int tries=1;
	for(i=0;i<X*Y;i++) tries*=3;
	for(try=0;try<tries;try++) {
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
