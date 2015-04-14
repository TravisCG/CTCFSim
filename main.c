#include <stdio.h>
#include <stdlib.h>
#include "random.h"

int *vector;
int *shuffled;

void copy2vec(){
	int i;

	for(i = 0; i < 30000; i++){
		shuffled[i] = vector[i];
	}
}

int main(){
	long i;
	int j;
	int swap;
	int rndpos, shuffpos;

	vector   = malloc(sizeof(int) * 30000);
	shuffled = malloc(sizeof(int) * 30000);
	if(vector == NULL){
		fprintf(stderr, "No enough memory for this simulation\n");
		return(1);
	}

	for(i = 0; i < 30000; i++){
		vector[i] = i;
	}

	initrnd();

	for(i = 0; i < 1e6; i++){
		copy2vec();

		for(j = 0; j < 30000; j++){
			rndpos           = getint(j, 30000-1);
			swap             = shuffled[j];
			shuffled[j]      = shuffled[rndpos];
			shuffled[rndpos] = swap;
		}
	}

	free(vector);
	free(shuffled);
	return(EXIT_SUCCESS);
}
