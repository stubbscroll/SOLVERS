/* episode 2 mission 3 photonic countdown phase 4 on expert */
/* enemy has 5 blockers with 4 hp, and 5 bombs with 3 hp each
   we have 14 drones, 3 animuses
   can buy tarsier (4R), perforator (3R), shadowfang (8RRR), immolite (3R),
   nitrocybe (1R), animus (6)
   (can also buy drones and engineers, but they're useless. a drone on turn 1
   gives 3 income, we'd rather have the same amount of gold earlier)
   the first 20 damage we deal must be in multiples of 4)
   trivial pruning:
   - don't buy drones and engineers (see above)
   - don't buy tarsiers and nitrocybes on turn 4 (they arrive too late)
   - don't buy tarsier on turn 3 (it only deals 1 damage on the last turn,
     nitrocybe does that cheaper)
   - on the last turn, immolites and perforators are equivalent
   - don't buy shadowfang on turn 4 (2 immolites are cheaper)
   possibly incorrect pruning:
   - spend all gold or red on every turn, or have empty nitrocybe supply
     (might not be optimal if we want to float gold for some reason)
   unproven pruning:
   - don't buy immolite on turn 3. it gets 1 attack ever, same as nitrocybe.
     only ever useful if our attack granularity sucks
   simplifications:
   - the only supply we care about is nitrocybe
*/
/* cumulative damage for each unit and turn built
   unit       1 2 3 4 (turn)
   tarsier    3 2 1 0
   perforator 4 3 2 1 (uses R, can be held)
   shadowfang 8 6 4 2
   immolite   2 2 1 1 (can be held)
   nitrocybe  1 1 1 0 (can be held)
*/

#include <stdio.h>

/* t[i][j]=n: in turn i, buy n of thing j, where j is:
   0:shadowfang
   1:tarsier
   2:immolite
   3:perforator
   4:nitrocybe
   5:animus
   6:gold left after buying
   7:attack
*/
int t[4][8];

void buy(int turn,int gold,int attack,int red,int shield,int bomb,int animus,int tarsierw,int tarsier,int perforator,int shadowfang,int immolitew,int immolite,int nitrocybew,int nitrocybe,int nsup);

/* turn: 0-4, turn number (0=start, 4=last turn)
   gold: number of gold we have
   shield: number of shields left
   bomb: number of bombs left
   animus: number of animuses
   tarsierw: number of tarsiers with buildtime 1
   tarsier: number of active tarsiers
   perforator: number of perforators
	 immolitew: number of immolites on exhaust
   immolite: number of active immolites
   nitrocybew: number of nitrocybes with buildtime 1
   nitrocybe: number of active nitrocybes
   nsup: number of nitrocybes left in supply
*/
void btr(int turn,int gold,int shield,int bomb,int animus,int tarsierw,int tarsier,int perforator,int shadowfang,int immolitew,int immolite,int nitrocybew,int nitrocybe,int nsup) {
	/* attack phase */
	if(turn==4) {
		/* if at turn 4 (0-indexed), attack with everything */
		/* can't attack with all perforators if we don't have enough red */
		int perf=perforator<animus*2?perforator:animus*2;
		/* we don't care any more about the number of stuff, just pass 0 */
		buy(turn,gold,tarsier+shadowfang*2+perf+immolite+nitrocybe,0,shield,bomb,animus,0,0,0,0,0,0,0,0,0);
	} else {
		/* otherwise, try all combinations of holding perforators and immolites, use
		   nitrocybes greedily as needed */
		for(int perf=0;perf<=perforator && perf<=animus*2;perf++) {
			for(int immo=0;immo<=immolite;immo++) {
				int attack=tarsier+shadowfang*2+perf+immo;
				int red=animus*2-perf;
				/* pad with nitrocybes to get multiple of 4 if there are shields left */
				if(shield*4>attack+nitrocybe && attack%4!=0 && nitrocybe>=(4-attack%4)) {
					buy(turn,gold,(attack+3)/4*4,red,shield,bomb,animus,0,tarsierw+tarsier,perforator,shadowfang,immo,immolitew+immolite-immo,0,nitrocybew+nitrocybe-(4-attack%4),nsup);
				} else if(shield*4>attack+nitrocybe) {
					/* attack with 0 nitrocybes, absorb if we can't attack 0 mod 4 */
					buy(turn,gold,attack,red,shield,bomb,animus,0,tarsierw+tarsier,perforator,shadowfang,immo,immolitew+immolite-immo,0,nitrocybew+nitrocybe,nsup);
				} else {
					/* we breach, use all nitrocybes */
					buy(turn,gold,attack+nitrocybe,red,shield,bomb,animus,0,tarsierw+tarsier,perforator,shadowfang,immo,immolitew+immolite-immo,0,nitrocybew,nsup);
				}
			}
		}
	}
}

/* attack: how much we attack with
   red: how much red we have left
*/
void buy(int turn,int gold,int attack,int red,int shield,int bomb,int animus,int tarsierw,int tarsier,int perforator,int shadowfang,int immolitew,int immolite,int nitrocybew,int nitrocybe,int nsup) {
	/* apply attack */
	/* here we assume that we breach on the last turn and ignore that bombs are
	   non-fragile. we might get a few false positives */
	int att=attack; /* attack before killing stuff */
	while(shield && attack>=4) {
		shield--;
		attack-=4;
	}
	while(shield<1 && bomb && attack>=3) {
		bomb--;
		attack-=3;
	}
	if(turn==4 && (0 || shield+bomb<1)) {
		/* we won */
		printf("yey, we won (%d). we bought: (shadow tars immo perf nitro ani gold-left atk)\n",attack);
		for(int i=0;i<4;i++) {
			printf("  turn %d:",i);
			for(int j=0;j<6;j++) printf(" %d",t[i][j]);
			printf(" => %d %d\n",t[i][6],t[i][7]);
		}
		printf("  turn 4: => %d\n",att);
		fflush(stdout);
	}
	if(turn==4) return;
	/* try all sensible ways of buying units */
	for(int shadow=0;shadow*3<=red && shadow*8<=gold;shadow++) {
		int red2=red-shadow*3;
		int gold2=gold-shadow*8;
		if(turn==3 && shadow>0) break;
		for(int tars=0;tars<=red2 && tars*4<=gold2;tars++) {
			if(turn>=2 && tars>0) break;
			int red3=red2-tars;
			int gold3=gold2-tars*4;
			for(int immo=0;immo<=red3 && immo*3<=gold3;immo++) {
				int red4=red3-immo;
				int gold4=gold3-immo*3;
				for(int perf=0;perf<=red4 && perf*3<=gold4;perf++) {
					int red5=red4-perf;
					int gold5=gold4-perf*3;
					/* on turn 3, there's no difference between perforator and immolite.
					   so buy immolites only */
					if(turn==3 && perf) break;
					for(int nitro=0;nitro<=red5 && nitro<=gold5 && nitro<=nsup;nitro++) {
						int gold6=gold5-nitro;
						int red6=red5-nitro;
						/* on turn 3, don't buy nitros as they don't arrive in time */
						if(turn==3 && nitro) break;
						for(int ani=0;ani*6<=gold6;ani++) {
							int gold7=gold6-ani*6;
							/* pruning! either spend all red or all gold, or let nitrocybe
							   supply run out. */
							if(red6>0 && gold7>0 && nsup>0) continue;
							t[turn][0]=shadow;
							t[turn][1]=tars;
							t[turn][2]=immo;
							t[turn][3]=perf;
							t[turn][4]=nitro;
							t[turn][5]=ani;
							t[turn][6]=gold7;
							t[turn][7]=att;
							btr(turn+1,gold7+14,shield,bomb,animus+ani,tarsierw+tars,tarsier,perforator+perf,shadowfang+shadow,immolitew,immolite+immo,nitrocybew+nitro,nitrocybe,nsup-nitro);
						}
					}
				}
			}
		}
	}
}

int main() {
	btr(0,14,5,5,3,0,0,0,0,0,0,0,0,20);
	return 0;
}
