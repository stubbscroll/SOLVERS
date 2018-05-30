/* solver for wave mechanics */
/* level format:
   line 1: x y (integers representing level size)
   line 2-y+1: lines with x*2 characters containing map. map must be
     converted to 1 colour
   each cell is 2 chars, char 1 is # or . where # is on (need to be toggled an
     odd number of times) and . is off (need to be toggled an even number of
     times), char 2 is either a number or . (no number)
*/
/* algorithm: do forced moves when available, otherwise do stupid backtracking
   and prune on unsolvable boards (this actually ended up being very fast) */

#include <stdio.h>
#include <stdlib.h>

int x,y;
char col[42][42]; /* [x][y], 1: odd flip, 0: even flip */
int num[42][42];  /* num as given on board, 0=no number */
int cur[42][42];  /* num on current board, 0=done (or there were none to begin with) */
int dir[42][42][10]; /* for each number, the dir we moved in (0-3) */

int dx[]={1,0,-1,0};
int dy[]={0,1,0,-1};
char dirstr[4][16]={"right","down","left","up"};

/* read puzzle from stdin */
void readinput() {
	int i,j;
	char s[81];
	if(scanf("%d %d",&x,&y)!=2) puts("parse error, expected x,y (ints)");
	if(x>40 || y>40) { puts("puzzle too large, max is 40*40"); exit(0); }
	/* fill board with sentinels */
	for(i=0;i<42;i++) for(j=0;j<42;j++) col[i][j]=-1;
	for(j=0;j<y;j++) {
		scanf("%80s",s);
		for(i=0;i<x;i++) {
			col[i+1][j+1]=s[i*2]=='#';
			cur[i+1][j+1]=num[i+1][j+1]=s[i*2+1]=='.'?0:s[i*2+1]-'0';
			if(num[i+1][j+1]<0 || num[i+1][j+1]>9) puts("error, num out of range");
		}
	}
}

void scan(int atx,int aty,int d,int *dist,int *number) {
	*dist=*number=0;
	while(1) {
		atx+=dx[d]; aty+=dy[d];
		if(col[atx][aty]<0) return; /* out of bounds, terminate */
		(*dist)++;
		if(cur[atx][aty]>0) {
			/* found number, return distance and number found */
			*number=cur[atx][aty];
			return;
		}
	}
}

/* easier interface for illegal() */
int scanok(int atx,int aty,int d) {
	int dist,number;
	scan(atx,aty,d,&dist,&number);
	return number>0 && dist<=number; /* true if cell in range of num */
}

void printboard() {
	int i,j;
	for(j=1;j<=y;j++) {
		for(i=1;i<=x;i++) printf("%c",col[i][j]?'#':'.');
		printf("\n");
	}
	printf("\n");
	for(j=1;j<=y;j++) {
		for(i=1;i<=x;i++) printf("%c",cur[i][j]?cur[i][j]+48:'.');
		printf("\n");
	}
	printf("\n");
}

/* check if current board is not solvable */
int illegal() {
	int i,j,d;
	for(i=1;i<=x;i++) for(j=1;j<=y;j++) if(col[i][j]) {
		/* num on cell = ok */
		if(cur[i][j]) goto ok;
		/* check all 4 dirs and declare board unsolvable if cell is unreacable
		   by all numbers. we assume no board has no pairs of numbers that can
		   reach each other */
		for(d=0;d<4;d++) if(scanok(i,j,d)) goto ok;
		/* unsolvable, unreachable on-cell */
		return 1;
	ok:;
	}
	return 0;
}

/* toggle n cells in direction d from atx,aty */
void toggle(int atx,int aty,int d,int n) {
	while(n--) {
		if(col[atx][aty]<0) return; /* out of bounds */
		col[atx][aty]^=1;
		atx+=dx[d];
		aty+=dy[d];
	}
}

void solved() {
	int i,j,k;
	printf("we are winner\n");
	for(j=1;j<=y;j++) for(i=1;i<=x;i++) if(num[i][j]) {
		printf("%d,%d:\n",i,j);
		for(k=num[i][j]-1;k>=0;k--) printf("  %d: %s\n",k+1,dirstr[dir[i][j][k]]);
	}
	exit(0);
}

void btr(int atx,int aty) {
	int i,j,d,x2,y2;
	if(illegal()) return; /* prune on unsolvable board */
	/* check if there are forced moves */
	for(i=1;i<=x;i++) for(j=1;j<=y;j++) if(col[i][j]) {
		int dist,number,dd=-1,n=-1;
		for(d=0;d<4;d++) {
			scan(i,j,d,&dist,&number);
			if(dist<number) goto notforced;
			if(dist==number && number) {
				/* cell at exact distance from number: candidate for forced */
				if(dd>-1) goto notforced;
				dd=d;
				n=number;
			}
		}
		if(dd>-1) {
			x2=i+dx[dd]*n;
			y2=j+dy[dd]*n;
			/* found forced move, apply it and recurse */
			toggle(i,j,dd,n+1);
			cur[x2][y2]--;
			dir[x2][y2][cur[x2][y2]]=dd^2;
			btr(atx,aty);
			cur[x2][y2]++;
			toggle(i,j,dd,n+1);
			return;
		}
	notforced:;
	}
	/* scan for next num */
	while(aty<=y && !cur[atx][aty]) {
		atx++;
		if(atx>x) {
			atx=1;
			aty++;
		}
	}
	if(aty>y) solved(); /* we won */
	/* try all dirs */
	for(d=0;d<4;d++) {
		/* next cell has to be in grid for move to be legal */
		if(col[atx+dx[d]][aty+dy[d]]<0) continue;
		toggle(atx,aty,d,cur[atx][aty]+1);
		cur[atx][aty]--;
		dir[atx][aty][cur[atx][aty]]=d;
		btr(atx,aty);
		cur[atx][aty]++;
		toggle(atx,aty,d,cur[atx][aty]+1);
	}
}

int main() {
	readinput();
	btr(1,1);
	puts("error, puzzle not solved");
	return 0;
}
