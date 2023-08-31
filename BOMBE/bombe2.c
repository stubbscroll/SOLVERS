/* solve 2 region situations
   - doesn't support variables or don't care sizes
   - doesn't help with generalization
   - supports the same clue types as the game, including ?
*/

/* input (via stdin):
   clue1 clue2
   s1 s12 s2
   where s12 is the size of the intersection of regions 1 and 2,
   s1 is size of region 1 outside of the intersection, similar with s2

   example:
   2+ 0/2
   3 5 2
*/

/* algorithm: dumb brute force: try all possible number of bombs in all
   3 regions, then check all solutions and output all new non-trivial regions
   (in all 7 possible output regions) that are expressible with game hints
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// sum of all sizes and all bombs should be below max
#define MAX 31
#define MAXSOL 30000

char clue[2][100];
int rs[3];

int cluemask[2]; // bitmask of clues: bit n set means region can have n bombs

int sol[MAXSOL][3];
int soln;

// warning, no error checking
void parseclue() {
	for(int i=0;i<2;i++) {
		int max=rs[1]+(i==0)?rs[0]:rs[2];
		cluemask[i]=0;
		char *p=clue[i];
		while(isdigit(*p)) p++;
		if(clue[i][0]=='!') cluemask[i]=~(1<<strtol(clue[i]+1,0,10));
		else if(!strcmp(p,"+")) {
			int n=strtol(clue[i],0,10);
			for(int j=n;j<=max;j++) cluemask[i]|=1<<j;
		} else if(!strcmp(p,"-")) {
			int n=strtol(clue[i],0,10);
			for(int j=n;j>=0;j--) cluemask[i]|=1<<j;
		} else if(!strcmp(clue[i],"2*")) {
			for(int j=0;j<=max;j+=2) cluemask[i]|=1<<j;
		} else if(!strcmp(p,"+2*")) {
			int n=strtol(clue[i],0,10);
			for(int j=n;j<=max;j+=2) cluemask[i]|=1<<j;
		} else if(*p=='/') {
			p=clue[i];
			while(1) {
				int n=0;
				while(isdigit(*p)) n=n*10+*(p++)-'0';
				cluemask[i]|=1<<n;
				if(*p!='/') break;
				p++;
			}
		} else if(clue[i][0]=='?') cluemask[i]=-1;
		else cluemask[i]=1<<strtol(clue[i],0,10);
	}
}

void cluesanity() {
	for(int i=0;i<2;i++) {
		printf("clue %d:",i+1);
		for(int j=0;j<MAX;j++) if(cluemask[i]&(1<<j)) printf(" %d",j);
		printf("\n");
	}
}

// dumb algorithm because it will run fast enough anyway
void solve() {
	soln=0;
	for(int r1=0;r1<=rs[0];r1++) for(int r2=0;r2<=rs[1];r2++) for(int r3=0;r3<=rs[2];r3++) {
		if(!(cluemask[0]&(1<<(r1+r2)))) continue;
		if(!(cluemask[1]&(1<<(r2+r3)))) continue;
//		printf("%d %d %d\n",r1,r2,r3);
		sol[soln][0]=r1;
		sol[soln][1]=r2;
		sol[soln++][2]=r3;
		if(soln==MAXSOL) puts("too many solutions"),exit(1);
	}
}

void output() {
	if(soln==0) puts("no solutions"),exit(1);
	for(int set=1;set<8;set++) {
		int okmask=0,nots=0;
		for(int j=0;j<soln;j++) {
			int n=0;
			for(int k=0;k<3;k++) if(set&(1<<k)) n+=sol[j][k];
			okmask|=1<<n;
		}
		// reject when output region equals input clue
		if(set==3 && okmask==cluemask[0]) continue;
		if(set==6 && okmask==cluemask[1]) continue;
		// output the strongest output clue
		// check for exact
		int bc=0;
		int maxsize=0;
		int min=MAX,max=-1;
		for(int k=0;k<3;k++) if(set&(1<<k)) maxsize+=rs[k];
		for(int i=0;i<=maxsize;i++) {
			if(okmask&(1<<i)) {
				bc++;
				max=i;
				if(min==MAX) min=i;
			}
		}
		// reject if region size is 0
		if(maxsize==0) continue;
		printf("region %d%d%d:\n",set&1,(set&2)>>1,(set&4)>>2);
//		printf("okmask %d, bc %d, min %d, max %d\n",okmask,bc,min,max);
		int mid=(max+min)/2;
		if(bc==1) { printf("%d (exact)\n",min); goto not; }
		else if(bc==2 && max-min<4) { printf("%d/%d\n",min,max); goto not; }
		else if(bc==3 && (max-min==4 || max-min==2) && (okmask&(1<<mid))) {
			printf("%d/%d/%d\n",min,mid,max);
			goto not;
		}
		// parity
		int ok=1;
		for(int i=min;i<=maxsize;i+=2) if(!(okmask&(1<<i))) ok=0;
		for(int i=min+1;i<=maxsize;i+=2) if(okmask&(1<<i)) ok=0;
		if(ok) { printf("%d+2*\n",min); goto not; }
		// plus
		ok=1;
		for(int i=min;i<=maxsize;i++) if(!(okmask&(1<<i))) ok=0;
		for(int i=0;i<min;i++) if(okmask&(1<<i)) ok=0;
		if(ok && min>0) { printf("%d+\n",min); goto not; }
		// minus
		ok=1;
		for(int i=min;i>=0;i--) if(!(okmask&(1<<i))) ok=0;
		for(int i=min+1;i<=maxsize;i++) if(okmask&(1<<i)) ok=0;
		if(ok && max<maxsize) { printf("%d-\n",min); goto not; }
	not:
		// not clues are treated separately
		// only nots between existing numbers are interesting
		for(int i=min;i<=max;i++) if(!(okmask&(1<<i))) printf("!%d ",i),nots=1;
		if(nots) printf("\n");
	}
}

int main() {
	scanf("%99s %99s",clue[0],clue[1]);
	scanf("%d %d %d",&rs[0],&rs[1],&rs[2]);
	if(rs[0]+rs[1]+rs[2]>=MAX) puts("regions too large"),exit(1);
	parseclue();
	cluesanity();
	solve();
	output();
	return 0;
}
