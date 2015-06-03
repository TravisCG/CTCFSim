#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
	readed = getline(&line, &linelen, csv); /* read header */
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

void mean(double *first, double *second, double *third){
	int i;
	double s1 = 0.0, s2 = 0.0, s3 = 0.0;

	for(i = 0; i < count; i++){
		s1 += shuffled[i] - shuffled[i + count];             /*CTCF - Rad21*/
		s2 += shuffled[i] - shuffled[i + 2 * count];         /*CTCF - Smc3 */
		s3 += shuffled[i + count] - shuffled[i + 2 * count]; /*Rad21 - Smc3*/
	}

	*first  = s1 / (double)count;
	*second = s2 / (double)count;
	*third  = s3 / (double)count;
}

void sd(double *first, double *second, double *third){
	int i;
	double d1, d2, d3;
	double mean1, mean2, mean3;
	double s1 = 0.0, s2 = 0.0, s3 = 0.0;

	mean(&mean1, &mean2, &mean3);

	for(i = 0; i < count; i++){
		d1 = shuffled[i] - shuffled[i + count];
		d2 = shuffled[i] - shuffled[i + 2 * count];
		d3 = shuffled[i + count] - shuffled[i + 2 * count];

		s1 += (d1 - mean1) * (d1 - mean1);
		s2 += (d2 - mean2) * (d2 - mean2);
		s3 += (d3 - mean3) * (d3 - mean3);
	}

	*first  = sqrt(s1 / (double)(count - 1));
	*second = sqrt(s2 / (double)(count - 1));
	*third  = sqrt(s3 / (double)(count - 1));
}

int main(int argc, char **argv){
	long i;
	int j;
	int swap;
	int rndpos;
	double threshold1, threshold2, threshold3;
	double sd1, sd2, sd3;

	if(argc < 2){
		fprintf(stderr, "Not enough parameter\n");
		return(2);
	}
	readtable(argv[1]);

	initrnd();
	copy2vec();

	sd(&threshold1, &threshold2, &threshold3);
	printf("cycle\tdiff1\tdiff2\tdiff3\n");
	for(i = 0; i < SIMNUM; i++){
		sd(&sd1, &sd2, &sd3);

		if(sd1 <= threshold1 && sd2 <= threshold2 && sd3 <= threshold3){
			printf("%f\t%f\t%f\n", sd1, sd2, sd3);
		}

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
