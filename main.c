#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"

#define SIMNUM 10
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

double sd(int start, int end, double mean){
	int i;
	double sum = 0.0;

	for(i = start; i < end; i++){
		sum += (shuffled[i] - mean) * (shuffled[i] - mean);
	}

	return(sqrt(sum / (double)(end - start - 1)));
}

void threscalc(double *th1, double *th2, double *th3){
	int i;

	double m1 = 0.0, m2 = 0.0, m3 = 0.0, diff, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;

	for(i = 0; i < count; i++){
		m1 += shuffled[i + count] - shuffled[i];
		m2 += shuffled[i + count * 2] - shuffled[i];
		m3 += shuffled[i + count * 2] - shuffled[i + count];
	}

	m1 = m1 / (double)count;
	m2 = m2 / (double)count;
	m3 = m3 / (double)count;

	for(i = 0; i < count; i++){
		diff = shuffled[i + count] - shuffled[i];
		sum1 += (diff - m1) * (diff - m1);

		diff = shuffled[i + count * 2] - shuffled[i];
		sum2 += (diff - m2) * (diff - m2);

		diff = shuffled[i + count * 2] - shuffled[i + count];
		sum3 += (diff - m3) * (diff - m3);
	}

	*th1 = sqrt( sum1 / (double)(count - 1));
	*th2 = sqrt( sum2 / (double)(count - 1));
	*th3 = sqrt( sum3 / (double)(count - 2));
}

void howmany(double th1, double th2, double th3){
	int i;
	int found = 0;

	for(i = 0; i < count; i++){
		if( (shuffled[i + count]     - shuffled[i]         <= th1) && 
		    (shuffled[i + count * 2] - shuffled[i]         <= th2) &&
		    (shuffled[i + count * 2] - shuffled[i + count] <= th3)){
			found++;
		}
	}
	fprintf(stderr, "%d\n", found);
}

void hist(int cycle){
	int i;
	for(i = 0; i < count; i++){
		printf("%d\t%d\t%d\t%d\n", cycle, shuffled[i] - shuffled[count + i], shuffled[i] - shuffled[count * 2 + i], shuffled[count + i] - shuffled[count * 2 + i]);
	}
}

int main(int argc, char **argv){
	long i;
	int j;
	int swap;
	int rndpos;
	double threshold1, threshold2, threshold3;

	if(argc < 2){
		fprintf(stderr, "Not enough parameter\n");
		return(2);
	}
	readtable(argv[1]);

	initrnd();
	copy2vec();

	threscalc(&threshold1, &threshold2, &threshold3);
	printf("cycle\tdiff1\tdiff2\tdiff3\n");
	for(i = 0; i < SIMNUM; i++){
		//howmany(threshold1, threshold2, threshold3);
		hist(i);
		/* Knuth shuffle in three groups */
		for(j = 0; j < count; j++){
			rndpos                  = getint(j, count - 1);
			swap                    = shuffled[j];
			shuffled[j]             = shuffled[rndpos];
			shuffled[rndpos]        = swap;

			rndpos                  = getint(count, count * 2 - 1);
			swap                    = shuffled[count + j];
			shuffled[count + j]     = shuffled[rndpos];
			shuffled[rndpos]        = swap;

			rndpos                  = getint(count * 2, count * 3 - 1);
			swap                    = shuffled[count * 2 + j];
			shuffled[count * 2 + j] = shuffled[rndpos];
			shuffled[rndpos]        = swap;
		}
	}

	free(vector);
	free(shuffled);
	return(EXIT_SUCCESS);
}
