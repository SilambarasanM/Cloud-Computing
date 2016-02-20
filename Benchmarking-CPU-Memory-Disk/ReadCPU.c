#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>


#define NUM_THREADS 4
#define EXPERIMENT_COUNT 600
#define LONG_TYPE unsigned long long int
#define LONG_PRINT llu

LONG_TYPE i[4], f[4];
int j;

//Perform Floating Point OPerations
void *doFLOP(void *arg)
{
	LONG_TYPE *counter;
	counter = (LONG_TYPE *) arg;

	float g;
	float a = 0.12345, b = 2.3456, c = 34.567, d = 456.78, e = 5678.9;

	for(*counter = 0, g = 0.0; j <= EXPERIMENT_COUNT; *counter=*counter+52, g=g+1.0)
	{	//2 x 5 x 5 = 50 Operations
		g + a + a; g + a + b; g + a + c; g + a + d; g + a + e;
		g + b + a; g + b + b; g + b + c; g + b + d; g + b + e;
		g + c + a; g + c + b; g + c + c; g + c + d; g + c + e;
		g + d + a; g + d + b; g + d + c; g + d + d; g + d + e;
		g + e + a; g + e + b; g + e + c; g + e + d; g + e + e;
	}
	pthread_exit(NULL);
}

//Perform Integer OPerations
void *doIOP(void *arg)
{
	LONG_TYPE *counter;
	counter = (LONG_TYPE *) arg;

	int g;
	int a = 12345, b = 23456, c = 34567, d = 45678, e = 56789;

	for(*counter = 0, g = 0; j <= EXPERIMENT_COUNT; *counter=*counter+52, g++)
	{	//2 x 5 x 5 = 50 Operations
		g + a + a; g + a + b; g + a + c; g + a + d; g + a + e;
		g + b + a; g + b + b; g + b + c; g + b + d; g + b + e;
		g + c + a; g + c + b; g + c + c; g + c + d; g + c + e;
		g + d + a; g + d + b; g + d + c; g + d + d; g + d + e;
		g + e + a; g + e + b; g + e + c; g + e + d; g + e + e;
	}
	pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
	int k;

	if(argc < 2)
	{
		printf("Insufficient Arguments Error.\n");
		printf("Usage: %s <FLOPS|IOPS>\n", argv[0]);
		return -1;
	}

	pthread_t tID[NUM_THREADS];
	LONG_TYPE tCur1, tCur2, tCur3, tCur4, tPrev1, tPrev2, tPrev3, tPrev4;
	LONG_TYPE tDiff1, tDiff2, tDiff3, tDiff4;
	tCur1 = tCur2 = tCur3 = tCur4 = j = tPrev1 = tPrev2 = tPrev3 = tPrev4 = 0;

	if(strcmp(argv[1], "IOPS") == 0)
	{	
		printf("TIME, TH1_IOPS, TH2_IOPS, TH3_IOPS, TH4_IOPS TOTAL_IOPS\n");

		//pthread creation
		for(k = 0; k< NUM_THREADS; k++) pthread_create(&tID[k], NULL, doIOP, &i[k]);
		
		//Sampling the counter of each thread every second
		while(j <= EXPERIMENT_COUNT)
		{
			tCur1 = i[0]; tCur2 = i[1]; tCur3 = i[2]; tCur4 = i[3];
			tDiff1 = tCur1 - tPrev1; tDiff2 = tCur2 - tPrev2; tDiff3 = tCur3 - tPrev3; tDiff4 = tCur4 - tPrev4; 
			tPrev1 = tCur1; tPrev2 = tCur2; tPrev3 = tCur3; tPrev4 = tCur4; 
			printf("%d, %llu, %llu, %llu, %llu, %llu\n", j, tDiff1, tDiff2, tDiff3, tDiff4, (tDiff1 + tDiff2 + tDiff3 + tDiff4));
			fflush(stdout);
			sleep(1); j++;
		}

		//pthread join
		for (k = 0; k < NUM_THREADS; k++) pthread_join(tID[k], NULL);
	}

	else if(strcmp(argv[1], "FLOPS") == 0)
	{
		printf("TIME, TH1_FLOPS, TH2_FLOPS, TH3_FLOPS, TH4_FLOPS, TOTAL_FLOPS\n");

		//pthread creation
		for(k = 0; k< NUM_THREADS; k++) pthread_create(&tID[k], NULL, doFLOP, &f[k]);

		//Sampling the counter of each thread every second	
		while(j <= EXPERIMENT_COUNT)
		{
			tCur1 = f[0]; tCur2 = f[1]; tCur3 = f[2]; tCur4 = f[3];
			tDiff1 = tCur1 - tPrev1; tDiff2 = tCur2 - tPrev2; tDiff3 = tCur3 - tPrev3; tDiff4 = tCur4 - tPrev4; 
			tPrev1 = tCur1; tPrev2 = tCur2; tPrev3 = tCur3; tPrev4 = tCur4; 
			printf("%d, %llu, %llu, %llu, %llu, %llu\n", j, tDiff1, tDiff2, tDiff3, tDiff4, (tDiff1 + tDiff2 + tDiff3 + tDiff4));
			fflush(stdout);
			sleep(1); j++;
		}

		//pthread join
		for (k = 0; k < NUM_THREADS; k++) pthread_join(tID[k], NULL);

	}

	return 0;
}
