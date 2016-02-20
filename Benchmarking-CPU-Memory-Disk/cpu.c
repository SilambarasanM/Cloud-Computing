#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>


#define REPEAT 3
#define EXPERIMENT_COUNT 600
#define MICRO 1000000.0
#define GIGA 1000000000.0

long int NUM_LOOP = 1000000000;

//Perform Floating Point OPerations
void *doFLOP(void *arg)
{
	int i;
	float a = 0.123456, b = 2.34567, c = 34.5678, d = 456.789, e = 5678.91, g;

	for(i = 0, g = 0.0; i < NUM_LOOP; i++, g += 1.0)
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
	int i, a = 123456, b = 234567, c = 345678, d = 456789, e = 567891;

	for(i = 0; i < NUM_LOOP; i++)
	{	//2 x 5 x 5 = 50 Operations
		i + a + a; i + a + b; i + a + c; i + a + d; i + a + e;
		i + b + a; i + b + b; i + b + c; i + b + d; i + b + e;
		i + c + a; i + c + b; i + c + c; i + c + d; i + c + e;
		i + d + a; i + d + b; i + d + c; i + d + d; i + d + e;
		i + e + a; i + e + b; i + e + c; i + e + d; i + e + e;
	}
	pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
	int i, j, numThread;
	struct timeval tStart, tStop;
	double tElapsed[REPEAT], tAvg, tVar, tSD, numOperations, OpsPerSec;

	if(argc < 3)
	{
		printf("Insufficient Arguments Error.\n");
		printf("Usage: %s <FLOPS|IOPS> <NumberOfThreads>\n", argv[0]);
		return -1;
	}
	
	numThread = atoi(argv[2]);
	pthread_t tID[numThread];
	numOperations = numThread * NUM_LOOP * 51; //50 + 1 Loop Counter

	if(strcmp(argv[1], "IOPS") == 0)
	{
		for (j = 0; j < REPEAT; j++)
		{
			gettimeofday(&tStart, NULL);
	
			//pthread creation
			for(i = 0; i < numThread; i++) pthread_create(&tID[i], NULL, doIOP, NULL);

			//pthread join
			for (i = 0; i < numThread; i++) pthread_join(tID[i], NULL);

			gettimeofday(&tStop, NULL);
			tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec)  + (double)(tStop.tv_usec - tStart.tv_usec)/MICRO;
		}

	}
	else if(strcmp(argv[1], "FLOPS") == 0)
	{
		for (j = 0; j < REPEAT; j++)
		{
			gettimeofday(&tStart, NULL);
	
			//pthread creation
			for(i = 0; i < numThread; i++) pthread_create(&tID[i], NULL, doFLOP, NULL);

			//pthread join
			for (i = 0; i < numThread; i++) pthread_join(tID[i], NULL);

			gettimeofday(&tStop, NULL);
			tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec)  + (double)(tStop.tv_usec - tStart.tv_usec)/MICRO;
		}

	}

	//Adding the time samples
	for(i = 0; i < REPEAT; i++) tAvg+=tElapsed[i];

	//Calculating average
	tAvg/=REPEAT;

	//Calculating Variance and Standard Deviation
	for(i = 0; i < REPEAT; i++) tVar+=pow((tElapsed[i]-tAvg),2);

	tVar/=REPEAT;
	tSD = sqrt(tVar);

	//Caculating Operations per second
	OpsPerSec = (double) (numOperations/tAvg);

	printf("%s, %d, %.0f, %.3f s, %.3f s, %.3lf G%s\n",
		argv[1], numThread, numOperations, tAvg, tSD, OpsPerSec/GIGA, argv[1]);

	return 0;
}
