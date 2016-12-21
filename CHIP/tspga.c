/* solve tsp-type levels using genetic algorithm
   support teleports, but with lynx timing (no boost)
   input format:
   line 1: x y (map dimensions)
   lines 2-y+1: map data
   other lines:
     before a b (chip a to be taken before chip b, 0-indexed id)
     before2 x1,y1 x2,y2 (same, but with coordinates)
     time d (start time for level)
*/
/* legend:
   . floor
   : gravel
   $ chip
   # wall
   T CC1 (blue) teleport
   S starting place
   E end (can be multiple)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXCHIPS 500
#define MAXMAP 100
#define MAXTELEPORT 500
#define MAXCON 100
#define INF 100000

void error(char *s) {
	puts(s);
	exit(0);
}

int x,y;
char map[MAXMAP][MAXMAP];
int id[MAXMAP][MAXMAP]; /* given coordinate, return id */
int chips,chipx[MAXCHIPS],chipy[MAXCHIPS];
int teleports,teleportx[MAXCHIPS],teleporty[MAXCHIPS];
int goals,goalx[MAXCHIPS],goaly[MAXCHIPS];
int startx,starty;
int constr,con1[MAXCON],con2[MAXCON];
double starttime;

int dist[MAXCHIPS+2][MAXCHIPS+2]; /* pairwise distances between chips (and start and end) */

int dx[]={1,0,-1,0};
int dy[]={0,1,0,-1};

/* given teleport pos and dir we entered, return where we end up */
void teleportendup(int *x2,int *y2,int dir) {
	int tid=-1,i,try,tx,ty;
	for(i=0;i<teleports;i++) if(*x2==teleportx[i] && *y2==teleporty[i]) tid=i;
	if(tid<0) error("teleport not found");
	try=tid-1;
	while(1) {
		tx=teleportx[try]+dx[dir];
		ty=teleporty[try]+dy[dir];
		if(tx<0 || ty<0 || tx>=x || ty>=y || map[tx][ty]=='#') goto next;
		*x2=tx; *y2=ty; return;
	next:
		if(try==tid) { *x2=-1; return; } /* stuck */
		try--;
		if(try<0) try=teleports-1;
	}
}

/* i'm lazy and use dijkstra instead of trying to patch bfs to support edges
   with cost 2 (teleport moves) */
void dijkstra(int from) {
	static int dist2[MAXMAP][MAXMAP];
	static char taken[MAXMAP][MAXMAP];
	int i,j,n=chips+2,bestx,besty,best,d,x2,y2,cost;
/*	printf("find distances from %d:\n",from);*/
	for(i=0;i<n;i++) dist[from][i]=(i==from)?0:INF;
	for(j=0;j<y;j++) for(i=0;i<x;i++) dist2[i][j]=(id[i][j]==from)?0:INF;
	for(j=0;j<y;j++) for(i=0;i<x;i++) taken[i][j]=0;
	while(1) {
		best=INF,bestx=besty=-1;
		for(j=0;j<y;j++) for(i=0;i<x;i++) if(!taken[i][j] && best>dist2[i][j]) best=dist2[i][j],bestx=i,besty=j;
		if(bestx<0) break;
		taken[bestx][besty]=1;
		if(id[bestx][besty]>-1) {
			j=id[bestx][besty];
			if(dist[from][j]==INF) {
				dist[from][j]=dist2[bestx][besty];
/*				printf("  to %d: %d\n",j,dist2[bestx][besty]);*/
			}
		}
		if(map[bestx][besty]=='E') continue;
		for(d=0;d<4;d++) {
			x2=bestx+dx[d]; y2=besty+dy[d];
			if(x2<0 || y2<0 || x2>=x || y2>=y || map[x2][y2]=='#') continue;
			if(map[x2][y2]=='T') cost=2,teleportendup(&x2,&y2,d);
			else cost=1;
			if(x2<0) continue;
			if(dist2[x2][y2]>dist2[bestx][besty]+cost) dist2[x2][y2]=dist2[bestx][besty]+cost;
		}
	}
}

void readinput() {
	static char s[10000];
	int i,j,x1,y1,x2,y2,id1,id2;
	chips=teleports=goals=constr=0;
	startx=starty=-1;
	starttime=-100;
	fgets(s,9997,stdin);
	if(2!=sscanf(s,"%d %d",&x,&y)) error("x y expected on first line");
	for(i=0;i<y;i++) {
		fgets(s,9997,stdin);
		for(j=0;j<x;j++) map[j][i]=s[j];
	}
	/* process map */
	for(j=0;j<y;j++) for(i=0;i<x;i++) {
		switch(map[i][j]) {
		case 'S':
			if(startx>=0) error("start position defined multiple times");
			startx=i; starty=j; break;
		case 'E':
			if(goals==MAXCHIPS) error("number of goals exceeds MAXCHIPS");
			goalx[goals]=i; goaly[goals++]=j; break;
		case '$':
			if(chips==MAXCHIPS) error("number of chips exceeds MAXCHIPS");
			chipx[chips]=i; chipy[chips++]=j; break;
		case 'T':
			if(teleports==MAXCHIPS) error("number of teleports exceeds MAXCHIPS");
			teleportx[teleports]=i; teleporty[teleports++]=j; break;
		}
	}
	/* calculate id-map */
	for(j=0;j<y;j++) for(i=0;i<x;i++) id[i][j]=-1;
	for(i=0;i<chips;i++) id[chipx[i]][chipy[i]]=i;
	id[startx][starty]=chips;
	for(i=0;i<goals;i++) id[goalx[i]][goaly[i]]=chips+1;
	if(startx<0) error("no start position defined");
	if(goals<1) error("no goal");
	printf("map size: %d %d\n",x,y);
	for(i=0;i<chips;i++) printf("chip %d at (%d,%d)\n",i,chipx[i],chipy[i]);
	printf("%d goals, %d teleports\n",goals,teleports);
	/* calculate all-pairs shortest path */
	for(i=0;i<chips+2;i++) dijkstra(i);
	/* read the rest now that we have processed map */
	while(fgets(s,9997,stdin)) {
		if(!strncmp(s,"before",6)) {
			if(constr==MAXCON) error("MAXCON exceeded");
			if(2==sscanf(s,"before %d %d",&con1[constr],&con2[constr])) constr++;
			if(4==sscanf(s,"before2 %d,%d %d,%d",&x1,&y1,&x2,&y2)) {
				id1=id[x1][y1]; id2=id[x2][y2];
				if(id1<0 || id2<0) error("coordinate points to non-chip in before2");
				con1[constr]=id1; con2[constr++]=id2;
			}
		} else if(!strncmp(s,"starttime",9)) {
			sscanf(s,"starttime %lf",&starttime);
		}
	}
}

/* genetic algorithm!
   representation: permutation of chips
   fitness function: 1/(length of path+FITMAGIC)
*/

#define POP 500
#define CAND 15000
#define ELITISM 4
#define FITMAGIC 0

#define UNI1 0.2    /* probability of 1 parent having mutation */
#define UNI2 0.2    /* probability of 1 parent having different mutation */
/* probability of 2 parent crossovering is 1-above */

typedef struct {
	double fitness; /* fitness value */
	double cumul;
	int dataptr;    /* pointer to array of permutations */
	int cost;       /* actual path length */
} ind_t;

ind_t pop[POP+CAND];
int *popdata;     /* permutation data for population+candidates */

int compo(const void *A,const void *B) {
	const ind_t *a=A,*b=B;
	int i,*p,*q;
	if(a->fitness>b->fitness) return -1;
	if(a->fitness<b->fitness) return 1;
	/* if tie, sort on data */
	p=popdata+a->dataptr*chips;
	q=popdata+b->dataptr*chips;
	for(i=0;i<chips;i++) {
		if(p[i]<q[i]) return -1;
		if(p[i]>q[i]) return 1;
	}
	return 0;
}

int rand31() {
	return ((rand()&1)<<30)+((rand()&32767)<<15)+(rand()&32767);
}

double rand01() {
	return (double)rand31()/(1LL<<31);
}

double calcfitness(int cost) {
	return 1./(cost+FITMAGIC);
}

int findcost(int *p) {
	int cost=0,at=chips,i;
	for(i=0;i<chips;i++) cost+=dist[at][p[i]],at=p[i];
	return cost+dist[at][chips+1];
}

void printperm(int *p) {
	int i;
	for(i=0;i<chips;i++) printf("%d ",p[i]);
	putchar('\n');
}

void printind(int ix) {
	if(starttime>-10) printf("cost %d (%.1f) fitness %f dataptr %d\n",pop[ix].cost,starttime-pop[ix].cost*.2,pop[ix].fitness,pop[ix].dataptr);
	else printf("cost %d fitness %f dataptr %d\n",pop[ix].cost,pop[ix].fitness,pop[ix].dataptr);
	printperm(popdata+pop[ix].dataptr*chips);
}

/* force individual to satisfy constraints */
void fixind(int *p) {
	int k,i,j,l,again=1;
	while(again) {
		again=0;
		for(k=0;k<constr;k++) {
			for(i=0;i<chips;i++) if(p[i]==con1[k]) break;
			for(j=0;j<chips;j++) if(p[j]==con2[k]) break;
			if(i<j) continue;
			if(i==j) error("sanity error");
			/* we have i>j, move p[i] to before p[j] */
			for(l=i;l>=j+1;l--) p[l]=p[l-1];
			p[j]=con1[k];
			again=1;
		}
	}
}

void initpop() {
	int i,j,k,*p,t;
	for(i=0;i<POP;i++) {
		pop[i].dataptr=i;
		p=popdata+i*chips;
		/* generate random permutation */
		for(j=0;j<chips;j++) p[j]=j;
		for(j=chips-1;j>=1;j--) {
			k=rand()%(j+1);
			t=p[k]; p[k]=p[j]; p[j]=t;
		}
		fixind(p);
		pop[i].cost=findcost(p);
		pop[i].fitness=calcfitness(pop[i].cost);
	}
}

void normalizecumul() {
	double sum=0,z;
	int i;
	for(i=0;i<POP;i++) sum+=pop[i].fitness;
	for(z=i=0;i<POP;i++) pop[i].cumul=z/sum,z+=pop[i].fitness;
}

int pickparent() {
	double r=rand01();
	int lo=0,hi=POP,mid;
	/* find individual with lowest cumul satisfying cumul>=r */
	while(lo<hi) {
		mid=lo+(hi-lo)/2;
		if(pop[mid].cumul>=r) hi=mid;
		else lo=mid+1;
	}
	if(lo==POP) lo--;
	return lo;
}

int nextp;

/* copy individual (including permutation data) from from to to */
void copyind(int to,int from) {
	int i,*p,*q;
	pop[to]=pop[from];
	pop[to].dataptr=nextp;
	p=popdata+pop[from].dataptr*chips;
	q=popdata+pop[to].dataptr*chips;
	for(i=0;i<chips;i++) q[i]=p[i];
}

void compress() {
	static int map[POP+CAND];
	int i,j,*p,*q;
	for(i=0;i<POP+CAND;i++) map[i]=-1;
	for(i=0;i<POP;i++) map[pop[i].dataptr]=0;
	for(i=j=0;i<POP+CAND;i++) if(!map[i]) map[i]=j++;
	for(i=0;i<POP;i++) pop[i].dataptr=map[pop[i].dataptr];
	for(i=0;i<POP+CAND;i++) if(map[i]>-1) {
		p=popdata+i*chips;
		q=popdata+map[i]*chips;
		for(j=0;j<chips;j++) q[j]=p[j];
	}
}

void ga() {
	static char taken[MAXCHIPS];
	double prob;
	int gen=1,i,j,k,ix,iy,a,b,t,*p,*q,*r,rev;
	int best=-1;
	popdata=malloc(sizeof(int)*chips*(POP+CAND));
	if(!popdata) error("out of memory");
	initpop();
	while(1) {
		if(gen%100==0) printf("start of generation %d\n",gen);
		gen++;
		qsort(pop,POP,sizeof(ind_t),compo);
		if(best!=pop[0].cost) {
			best=pop[0].cost;
			printind(0);
		}
		normalizecumul();
		nextp=POP;
		for(i=0;i<CAND;i++) {
		again:
			prob=rand01();
			if(prob<UNI1) {
				/* mutation operator 1: swap 2 nodes */
				ix=pickparent();
				copyind(POP+i,ix);
				/* swap 2 random elements */
				a=rand()%chips;
				do b=rand()%chips; while(a==b);
				p=popdata+pop[POP+i].dataptr*chips;
				t=p[a]; p[a]=p[b]; p[b]=t;
				fixind(p);
				if(!compo(pop+POP+i,pop+ix)) goto again;  /* if no change, discard */
				pop[POP+i].cost=findcost(p);
				pop[POP+i].fitness=calcfitness(pop[POP+i].cost);
				nextp++;
			} else if(prob<UNI1+UNI2) {
				/* mutation operator 2: move 1 node */
				ix=pickparent();
				copyind(POP+i,ix);
				/* pick random element, and new position for it */
				a=rand()%chips;
				do b=rand()%chips; while(a==b);
				p=popdata+pop[ix].dataptr*chips;
				q=popdata+pop[POP+i].dataptr*chips;
				for(j=0;j<chips;j++) q[j]=-1;
				q[b]=p[a];
				for(j=k=0;j<chips;j++) if(j!=a) {
					while(k<chips && q[k]>-1) k++;
					if(k==chips) printf("sanity error");
					q[k++]=p[j];
				}
				fixind(q);
				pop[POP+i].cost=findcost(p);
				pop[POP+i].fitness=calcfitness(pop[POP+i].cost);
				nextp++;
			} else {
				/* crossover operator: copy substring from parent 1, take remaining
				   nodes from parent 2 in order of occurrence */
				ix=pickparent();
				do iy=pickparent(); while(ix==iy);
				copyind(POP+i,iy);
				a=rand()%chips;
				b=rand()%chips;
				if(a>b) t=a,a=b,b=t;
				b++;
				do t=rand()%chips; while(t+b-a>chips);
				rev=rand()&1;
				p=popdata+pop[ix].dataptr*chips;
				q=popdata+pop[iy].dataptr*chips;
				r=popdata+pop[POP+i].dataptr*chips;
				for(k=0;k<chips;k++) r[k]=-1;
				for(k=0;k<chips;k++) taken[k]=0;
				if(rev) for(j=0;j<b-a;j++) taken[p[j+a]]++,r[t+b-a-j-1]=p[j+a];
				else for(k=a;k<b;k++) taken[p[k]]++,r[k+t-a]=p[k];
				for(k=j=0;k<chips;k++) if(!taken[q[k]]) {
					while(j<chips && r[j]>-1) j++;
					if(j==chips) printf("sanity error");
					r[j++]=q[k];
				}
				fixind(r);
				if(!compo(pop+POP+i,pop+ix)) goto again;  /* if no change, discard */
				if(!compo(pop+POP+i,pop+iy)) goto again;  /* if no change, discard */
				pop[POP+i].cost=findcost(r);
				pop[POP+i].fitness=calcfitness(pop[POP+i].cost);
				nextp++;
			}
		}
		/* sort all children */
		qsort(pop+POP,CAND,sizeof(ind_t),compo);
		/* ensure we only have unique elitists */
		/* here shallow copy is ok! */
		if(ELITISM) for(j=i=1;i<ELITISM;i++) {
			if(compo(pop+i,pop+i-1)) pop[j++]=pop[i];
		} else j=0;
		pop[j++]=pop[POP];
		/* ensure we only have unique children */
		for(i=POP+1;j<POP && i<POP+CAND;i++) if(compo(pop+i,pop+i-1)) pop[j++]=pop[i];
		if(j<POP) error("ga failed, need more unique children");
		compress();
	}
}

int main() {
	srand(time(0));
	readinput();
	ga();
	return 0;
}
