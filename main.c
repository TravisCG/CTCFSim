#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "random.h"

#define SIMNUM 100e6
#define LAZYSIZE 5000

int *vector;
int *shuffled;
int count;

void copy2vec(){
	int i;

	for(i = 0; i < count * 3; i++){
		shuffled[i] = vector[i];
	}
}

void readtable(char *filename){
	FILE *csv;
	char *line = NULL;
	size_t linelen = 0;
	ssize_t readed;
	int *ctcf, *rad21, *smc3;

	ctcf  = malloc(sizeof(int) * LAZYSIZE);
	rad21 = malloc(sizeof(int) * LAZYSIZE);
	smc3  = malloc(sizeof(int) * LAZYSIZE);

	csv = fopen(filename, "r");
	getline(&line, &linelen, csv); /* read header */
	count = 0;
	while( (readed = getline(&line, &linelen, csv)) != -1){
		strtok(line, "\t");

		ctcf[count]  = atoi(strtok(NULL, "\t"));
		rad21[count] = atoi(strtok(NULL, "\t"));
		smc3[count]  = atoi(strtok(NULL, "\n"));
		count++;

		if(count % LAZYSIZE == 0){
			ctcf  = realloc(ctcf,  sizeof(int) * (count + LAZYSIZE));
			rad21 = realloc(rad21, sizeof(int) * (count + LAZYSIZE));
			smc3  = realloc(smc3,  sizeof(int) * (count + LAZYSIZE));
		}
	}
	fclose(csv);
	free(line);

	vector   = malloc(sizeof(int) * 3 * count);
	shuffled = malloc(sizeof(int) * 3 * count);

	memcpy(vector, ctcf, count * sizeof(int));
	memcpy((vector+count), rad21, count * sizeof(int));
	memcpy((vector+count+count), smc3, count * sizeof(int));

	free(ctcf);
	free(rad21);
	free(smc3);
}

double mean(int start, int end){
	int i;
	double sum = 0.0;

	for(i = start; i < end; i++){
		sum += shuffled[i];
	}

	return(sum / (double)(end - start));
}

void howmany(double th1, double th2, double th3){
	int i;
	int found = 0;

	for(i = 0; i < count; i++){
		if( (shuffled[i + count]     - shuffled[i]         >= th1) && 
		    (shuffled[i + count * 2] - shuffled[i]         >= th2) &&
		    (shuffled[i + count * 2] - shuffled[i + count] >= th3)){
			found++;
		}
	}
	fprintf(stderr, "%d\n", found);
}

int main(int argc, char **argv){
	long i;
	int j;
	int swap;
	int rndpos1, rndpos2;
	double m1, m2, m3;
	double o1, o2, o3, threshold1, threshold2, threshold3;
	int found = 0;

	if(argc < 2){
		fprintf(stderr, "Not enough parameter\n");
		return(2);
	}
	readtable(argv[1]);

	initrnd();
	copy2vec();

	o1 = mean(0, count);
	o2 = mean(count, count * 2);
	o3 = mean(count * 2, count * 3);

	threshold1 = o2 - o1;
	threshold2 = o3 - o1;
	threshold3 = o3 - o2;

	for(i = 0; i < SIMNUM; i++){
		m1 = mean(0, count);
		m2 = mean(count, count * 2);
		m3 = mean(count * 2, count * 3);

		if( (m2 - m1 >= threshold1) && (m3 - m1 >= threshold2) && (m3 - m2 >= threshold3)){
			found++;
		}

		howmany(threshold1, threshold2, threshold3);

		/* Modified Knuth shuffle */
		for(j = 0; j < count; j++){
			rndpos1               = getint(0, 2);
			rndpos2               = getint(0, 2);
			swap                  = shuffled[j * rndpos1];
			shuffled[j * rndpos1] = shuffled[j * rndpos2];
			shuffled[j * rndpos2] = swap;
		}
	}

	printf("$d\n", found);

	free(vector);
	free(shuffled);
	return(EXIT_SUCCESS);
}
