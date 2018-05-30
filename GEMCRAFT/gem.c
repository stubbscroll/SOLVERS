/* given amount of mana to spend, find best killgem by trying all ways to
   use different affordable combines. uses recipes from wgemcombiner.
   use it in endurance runs to build killgem after throwing manafarm into
   mana shard.
   to understand what's going on, read this:
   http://steamcommunity.com/sharedfiles/filedetails/?id=434889753
*/
/* usage:
   type 'gem' and then enter the amount of mana via standard input
   (e-notation like 1.234e56 is accepted).
   the last output is the best one found.
   - no recipe with steps >= STEPLIM is used. change STEPLIM if you want
     smaller or larger recipes
   - change the parameters to speccost in solve() to change base gem and number
     of amps
*/

#include <stdio.h>

#define MAX 100
#define STEPLIM 8000
#define MAXLEN 1000

int cost[MAX];
double growth[MAX];
double power[MAX];
int steps[MAX];
int n;

void insert(int c,double g,double p,int s) {
	if(s<=STEPLIM) {
		if(n==MAX) printf("increase max and recompile");
		cost[n]=c;
		growth[n]=g;
		power[n]=p;
		steps[n++]=s;
	}
}

void init() {
	n=0;
	insert(      6, 1.410077, 12.50987,   7);
	insert(      8, 1.399942, 18.37697,   9);
	insert(     11, 1.417164, 29.91054,  12);
	insert(     16, 1.414061, 50.43123,  17);
	insert(     30, 1.424546, 127.12373, 22);
	insert(     32, 1.423699, 138.95693, 24);
	insert(     56, 1.42828,  313.97942, 27);
	insert(     64, 1.427442, 378.6317,  38);
	insert(    128, 1.431338, 1037.83776, 57);
	insert(    256, 1.434242, 2844.453, 62);
	insert(    512, 1.435661, 7755.18228, 94);
	insert(   1024, 1.437381, 21229.95274, 131);
	insert(   2048, 1.438582, 58025.52415, 128);
	insert(   2628, 1.439846, 83894.7865,  167);
	insert(   4096, 1.439507, 158496.3311, 170);
	insert(   5376, 1.44125,  237970.35538, 176);
	insert(   8192, 1.440481, 433673.44498, 227);
	insert(  12497, 1.442339, 810929.75183, 254);
	insert(  16384, 1.441497, 1188705.05927, 368);
	insert(  24157, 1.443506, 2122992.70448, 313);
	insert(  32768, 1.442596, 3265639.37524, 1092);
	insert(  52273, 1.444261, 6522621.65425, 577);
	insert(  65536, 1.443811, 8996754.15543, 2017);
	insert( 105149, 1.445208, 18094679.79434, 1438);
	insert( 131072, 1.444854, 24777470.69941, 3484);
	insert( 218917, 1.44593,  52682827.33327, 2979);
	insert( 262144, 1.445686, 68156356.3557, 7115);
	insert( 383431, 1.445528, 117863281.2731, 402);
	insert( 524288, 1.444112, 181844662.75382, 5694);
	insert( 866147, 1.446079, 385679108.55414, 3300);
	insert(1048576, 1.44589,  507137900.33233, 4528);
	insert(1594158, 1.446686, 939977562.86719, 3891);
	insert(2097152, 1.446300, 1389877271.45088, 51903);
	insert(4062424, 1.446953, 3652606641.40712, 597);
	insert(4194304, 1.446793, 3816055882.85456, 26810);
}

int speccost(int kill,int numamp,int amp) {
	return kill*6+(kill-1)*48+numamp*(amp*6+(amp-1)*48);
}

double combinecost(double curcost,int mul) {
	return curcost*mul+(mul-1)*48*9;
}

double best;
int bestpath[MAXLEN],bpn;
int path[MAXLEN];

/* stupid backtracking, could be made much faster but runtime is tolerable and
   optimal answer is found early anyway */
void btr(double curcost,int minix,double curpower,double manalim,int at) {
	int i,totstep;
	double newcost;
	if(at==MAXLEN) printf("error");
	if(curpower>best) {
		best=curpower;
		bpn=at;
		for(i=0;i<at;i++) bestpath[i]=path[i];
		printf("power %.4e cost %.4e",curpower,curcost);
		for(i=0;i<at;i++) printf(" %dc",cost[bestpath[i]]);
		for(totstep=i=0;i<at;i++) totstep+=steps[bestpath[i]];
		printf(" => %d steps (%d:%02d)\n",totstep,totstep/20/60,totstep/20%60);
		printf("\n");
	}
	for(i=n-1;i>=minix;i--) {
		if(at==0) printf("- try %dc\n",cost[i]);
		newcost=combinecost(curcost,cost[i]);
		if(newcost<=manalim) path[at]=i,btr(newcost,i,curpower*power[i],manalim,at+1);
	}
}

void solve(double mana) {
	/* assume best spec */
	best=0;
	btr(speccost(1998,8,108),0,33949.84278,mana,0); /* normal killgem */
//	btr(160272,0,35157.81555,mana,0); /* GES */
//	btr(4.046e21,0,35157.81555,mana,0); /* custom */
}

int main() {
	double mana;
	init();
	scanf("%lf",&mana);
	solve(mana);
	return 0;
}
