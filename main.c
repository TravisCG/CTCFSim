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
	int i;

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

int main(int argc, char **argv){
	long i;
	int j;
	int swap;
	int rndpos;
	double m1, m2, m3;
	double o1, o2, o3, threshold1, threshold2, threshold3;
	int bin;

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

		if( (m2 - m1 >= threshold1) && (m3 - m1 >= threshold2)){
			printf("%f\t%f\t%f\n", m1, m2, m3);
		}
		/* Honestly, I can not understand the logic behind this *
		bin = 0;
		if((m1 > o1 - 1.0 && m1 < o1 + 1.0) || (m1 > o2 - 1.0 && m1 < o2 + 1.0) || (m1 > o3 - 1.0 && m1 < o3 + 1.0)){
			bin++;
		}
		if((m2 > o1 - 1.0 && m2 < o1 + 1.0) || (m2 > o2 - 1.0 && m2 < o2 + 1.0) || (m2 > o3 - 1.0 && m2 < o3 + 1.0)){
			bin++;
		}
		if((m3 > o1 - 1.0 && m3 < o1 + 1.0) || (m3 > o2 - 1.0 && m3 < o2 + 1.0) || (m3 > o3 - 1.0 && m3 < o3 + 1.0)){
			bin++;
		}

		if(bin > 0){
			printf("%f\t%f\t%f\t%d\n", m1, m2, m3, bin);
		}
		*/
		/* Knuth shuffle */
		for(j = 0; j < count * 3; j++){
			rndpos           = getint(j, (count * 3)-1);
			swap             = shuffled[j];
			shuffled[j]      = shuffled[rndpos];
			shuffled[rndpos] = swap;
		}

	}

	free(vector);
	free(shuffled);
	return(EXIT_SUCCESS);
}
