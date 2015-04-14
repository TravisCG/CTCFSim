/*
  This code is a mix of ran2 routine in "Numerical Recipies in C" by
  Flannery, Teukolsky and Verrerling,
  and Edward H. Trager's shuffle:

  http://eyegene.ophthy.med.umich.edu/shuffle/

*/
#include <stdio.h>
#include <stdlib.h>

#define IM1 2147483563
#define IM2 2147483399
#define IMM1 IM1-1
#define IA1  40014
#define IA2  40692
#define IQ1  53668
#define IQ2  52774
#define IR1  12211
#define IR2  3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 3.0e-16
#define RNMX (1.0 - EPS)
#define AM (1.0/IM1)

int idum;
int idum2;
int k;
int iv[NTAB];
int iy;

void initrnd(){
	FILE *seedfile;
	int i;

	seedfile = fopen("/dev/random", "r");
	fread(&idum, sizeof(int), 1, seedfile);
	fclose(seedfile);

	idum = idum == 0 ? 1 : -idum;
	idum2 = idum;

	for(i = NTAB + 7; i >= 0; i--){
		k = idum / IQ1;
		idum = IA1 * (idum - k * IQ1) - k * IR1;
		if(idum < 0) idum += IM1;
		if(i < NTAB) iv[i] = idum;
	}
	iy = iv[0];
}

double getrnd(){
	int i;
	double res;

	k = idum / IQ1;
	idum = IA1 * (idum - k * IQ1) - k * IR1;
	if(idum < 0) idum += IM2;
	i = iy / NDIV;
	iy = iv[i] - idum2;
	iv[i] = idum;
	if(iy < 1) iy += IMM1;
	if( (res = AM * iy) > RNMX) return RNMX;
	return res;
}

int getint(int low, int high){
	return low + (int)((high - low+1) * getrnd());
}
