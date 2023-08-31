/* solve 3 region situations
   - doesn't support variables or don't care sizes
   - doesn't help with generalization
   - supports the same clue types as the game, including ?
*/

/* input (via stdin):
   clue1 clue2 clue3
   s1  s12  s2
   s13 s123 s23 s3

   this is how the rules are shown in the game as well, except for the
   placement of the third clue

   example:
   !5 4/7 3
   3 4 0
   0 0 3 3
*/

/* algorithm: dumb brute force: try all possible number of bombs in all
   7 regions, then check all solutions and output all new non-trivial regions
   (in all 255 possible output regions) that are expressible with game hints
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// sum of all sizes and all bombs should be below max
#define MAX 31

char clue[3][100];
int rs[8]; // index: bit i is set if region i is involved
// 1 3 2      1  4  2
// 5 7 6 4 => 16 64 32 8

int cluemask[3]; // bitmask of clues: bit n set means region can have n bombs

// bitmask of solutions: index is into the powerset of all possible
// intersections of the 3 input regions
int solmask[128];

// warning, no error checking
void parseclue() {
	for(int i=0;i<3;i++) {
		int max=0;
		for(int j=0;j<8;j++) if(j&(1<<i)) max+=rs[j];
		cluemask[i]=0;
		char *p=clue[i];
		while(isdigit(*p)) p++;
		if(clue[i][0]=='!') {
			cluemask[i]=0;
			int n=strtol(clue[i]+1,0,10);
			for(int j=0;j<=max;j++) if(j!=n) cluemask[i]|=1<<j;
		} else if(!strcmp(p,"+")) {
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
	for(int i=0;i<3;i++) {
		printf("clue %d:",i+1);
		for(int j=0;j<MAX;j++) if(cluemask[i]&(1<<j)) printf(" %d",j);
		printf("\n");
	}
}

// the same dumb algorithm because it's still fast enough
// except i made the storing of solutions better
void solve() {
	for(int r1=0;r1<=rs[1];r1++) {
		for(int r12=0;r12<=rs[3];r12++) {
			for(int r13=0;r13<=rs[5];r13++) {
				for(int r123=0;r123<=rs[7];r123++) {
					// prune early if number of bombs in region 1 is wrong
					if(!(cluemask[0]&(1<<(r1+r12+r13+r123)))) continue;
					for(int r2=0;r2<=rs[2];r2++) {
						for(int r23=0;r23<=rs[6];r23++) {
							if(!(cluemask[1]&(1<<(r2+r12+r23+r123)))) continue;
							for(int r3=0;r3<=rs[4];r3++) {
								if(!(cluemask[2]&(1<<(r3+r13+r23+r123)))) continue;
								// we have a valid solution
								int s[7];
								s[0]=r1; s[1]=r2; s[2]=r12;
								s[3]=r3; s[4]=r13; s[5]=r23; s[6]=r123;
								// for each element in the power set of the 7 intersections,
								// mark number of bombs in subregion in solutions
								for(int p=0;p<128;p++) {
									int n=0;
									for(int i=0;i<7;i++) if(p&(1<<i)) n+=s[i];
									solmask[p]|=1<<n;
								}
//								printf("%d %d %d\n%d %d %d %d\n",r1,r12,r2,r13,r123,r23,r3);
							}
						}
					}
				}
			}
		}
	}
}

void printregion(int set) {
	printf("region %c%c%c\n       %c%c%c%c (%d) => ",(set&1)?'*':'.',
	       (set&(1<<2))?'*':'.',(set&(1<<1))?'*':'.',(set&(1<<4))?'*':'.',
	       (set&(1<<6))?'*':'.',(set&(1<<5))?'*':'.',(set&(1<<3))?'*':'.',set);
}

void output() {
	for(int set=1;set<128;set++) {
		// reject any set which contains a subregion of size 0
		int size0=0;
		for(int k=0;k<7;k++) if(set&(1<<k)) if(rs[k+1]==0) size0=1;
		if(size0) continue;
		// reject when output region equals input clue
		if((set&85)==set && cluemask[0]==solmask[set]) continue;
		if((set&102)==set && cluemask[1]==solmask[set]) continue;
		if((set&120)==set && cluemask[2]==solmask[set]) continue;
		int bc=0,maxsize=0,min=MAX,max=-1;
		for(int k=0;k<7;k++) if(set&(1<<k)) maxsize+=rs[k+1];
		for(int i=0;i<=maxsize;i++) if(solmask[set]&(1<<i)) {
			bc++;
			max=i;
			if(min==MAX) min=i;
		}
		// reject if region size is 0
		if(maxsize==0) continue;
		// reject if output is trivial (all number of mines possible)
		if(bc==maxsize+1) continue;
//		printf("maxsize %d, bc %d, min %d, max %d\n",maxsize,bc,min,max);
		// output strongest solution
		// exact
		if(bc==1) { printregion(set); printf("%d (exact)\n",min); continue; }
		// x/x+?
		if(bc==2 && max-min<4) { printregion(set); printf("%d/%d\n",min,max); continue; }
		// x/x+?/x+2?
		if(bc==3 && (max-min==2 || max-min==4)) {
			int mid=(min+max)/2;
			if(solmask[set]&(1<<mid)) {
				printregion(set);
				printf("%d/%d/%d\n",min,mid,max);
				continue;
			}
		}
		// parity
		int ok=1;
		for(int i=min;i<=maxsize;i+=2) if(!(solmask[set]&(1<<i))) ok=0;
		for(int i=min+1;i<=maxsize;i+=2) if(solmask[set]&(1<<i)) ok=0;
		if(ok) { printregion(set); printf("%d+2*\n",min); continue; }
		// plus
		ok=1;
		for(int i=min;i<=maxsize;i++) if(!(solmask[set]&(1<<i))) ok=0;
		for(int i=0;i<min;i++) if(solmask[set]&(1<<i)) ok=0;
		if(ok) { printregion(set); printf("%d+\n",min); continue; }
		// minus
		ok=1;
		for(int i=min;i>=0;i--) if(!(solmask[set]&(1<<i))) ok=0;
		for(int i=min+1;i<=maxsize;i++) if(solmask[set]&(1<<i)) ok=0;
		if(ok) { printregion(set); printf("%d-\n",min); continue; }
		// not
		int nots=0;
		// only nots between existing numbers are interesting
		for(int i=min;i<=max;i++) if(!(solmask[set]&(1<<i))) {
			if(!nots) printregion(set); 
			printf("!%d ",i);
			nots=1;
		}
		if(nots) printf("\n");
	}
}

int main() {
	scanf("%99s %99s %99s",clue[0],clue[1],clue[2]);
	scanf("%d %d %d %d %d %d %d",&rs[1],&rs[3],&rs[2],&rs[5],&rs[7],&rs[6],&rs[4]);
	if(rs[1]+rs[2]+rs[3]+rs[4]+rs[5]+rs[6]+rs[7]>=MAX) puts("regions too large"),exit(1);
	parseclue();
	cluesanity();
	solve();
	output();
	return 0;
}
