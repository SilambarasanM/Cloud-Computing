#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#define REPEAT 3
#define BYTE 1
#define KILOBYTE 1024 * 1
#define MEGABYTE 1024 * 1024 * 1
#define MICRO 1000000.0
#define INT_SIZE sizeof(int)
#define CHAR_SIZE sizeof(char)
#define NUM_LOOP_MB 100
#define NUM_LOOP_KB 100000
#define NUM_LOOP_B 99000000

int numBytes, dataSize, *numRand, NUM_LOOP;
char *write, *read;

//Debug function only for debug purpose
void debug(char* data)
{
	int i;
	printf("Visualizing Data...\n");
	for(i = 0; i < dataSize; i++)
	{
		if(i%numBytes ==0) printf("\n");
		if(data[i] == '0')
			printf("%c",data[i]);
		else
			printf("~");

	}
	printf("\nExiting with i=%d\n",i);
}

//Perform Sequential Read/Write
void *doSEQReadWrite(void *arg) 
{
	int i;
	
	char* data = (char*) malloc(dataSize);
	char* write = (char*) arg;

	for (i = 0; i < NUM_LOOP; i++) 
	{
		memcpy(&data[i*numBytes], write, numBytes);
	}
	free(data);	
	pthread_exit(NULL);
}

//Perform Random Read/Write
void *doRANDReadWrite(void *arg) 
{
	int i, index;
	char* data = (char*) malloc(dataSize);
	char* write = (char*) arg;
	
	for (i = 0; i < NUM_LOOP; i++) 
	{
		index = *(numRand+i);
		memcpy(&data[index*numBytes], write, numBytes);
	}
	
	free(data);	
	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) 
{
	int i, j, numThread;
	struct timeval tStart, tStop;
	double tElapsed[REPEAT], tTotal, tAvg, tVar, tSD, totalBytes, bps, tRandTime, tLatency;

	if(argc < 4)
	{
		printf("Insufficient Arguments Error.\n");
		printf("Usage: %s <SEQ|RAND> <1B|1KB|1MB> <NumberOfThreads>\n", argv[0]);
		return -1;
	}
	
	numThread = atoi(argv[3]);

	//Number of bytes given in CLI argument
	if(strcmp(argv[2],"1B") == 0) 
	{
		numBytes = BYTE;
		NUM_LOOP = NUM_LOOP_B;
	}
	else if(strcmp(argv[2],"1KB") == 0)
	{
		numBytes = KILOBYTE;
		NUM_LOOP = NUM_LOOP_KB;
	}
	else if(strcmp(argv[2],"1MB") == 0)
	{
		numBytes = MEGABYTE;
		NUM_LOOP = NUM_LOOP_MB;
	}

	//Total number of bytes processed
	totalBytes = numThread * NUM_LOOP * numBytes * CHAR_SIZE;

	pthread_t tIDRead[numThread], tIDWrite[numThread];

	//Size of data memory block per thread	
	dataSize = numBytes * NUM_LOOP * sizeof(char);

	//Data to be written and read
	write = (char*) malloc(numBytes * sizeof(char));
	read = (char*) malloc(numBytes * sizeof(char));

	for(j = 0; j < numBytes; j++)
		memset(&write[j], (char) (32 + rand() % 95), CHAR_SIZE);
	
	//Random numbers to be used for random access
	numRand = (int*) malloc(NUM_LOOP * INT_SIZE);
	for(j = 0; j < NUM_LOOP; j++) *(numRand+j) = rand() % NUM_LOOP;

	
	if(strcmp(argv[1], "SEQ") == 0)
	{
		for (j = 0; j < REPEAT; j++)
		{
			gettimeofday(&tStart, NULL);
			//pthread creation for write operation
			for(i = 0; i < numThread; i++) 
				pthread_create(&tIDWrite[i], NULL, doSEQReadWrite, (void*)write);

			//pthread creation for write operation
			for (i = 0; i < numThread; i++)	pthread_join(tIDWrite[i], NULL);
			
			gettimeofday(&tStop, NULL);

			tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
		}
	}
	else if(strcmp(argv[1], "RAND") == 0)
	{
		for (j = 0; j < REPEAT; j++)
		{
			gettimeofday(&tStart, NULL);

			//pthread creation for write operation	
			for(i = 0; i < numThread; i++) 
				pthread_create(&tIDWrite[i], NULL, doRANDReadWrite, (void*)write);

			//pthread creation for write operation
			for (i = 0; i < numThread; i++) pthread_join(tIDWrite[i], NULL);

			gettimeofday(&tStop, NULL);

			tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
		}
	}

	//Releasing allocated memory
	free(write); free(read);

	//Adding the times samples
	for(i = 0; i < REPEAT; i++) 
		tTotal+=tElapsed[i]; 

	//Calculating average
	tAvg=tTotal/REPEAT;
		
	//Calculating Variance and Standard Deviation
	for(i = 0; i < REPEAT; i++) 
		tVar+=pow(((tElapsed[i])-tAvg),2);

	tVar/=REPEAT; tSD = sqrt(tVar);
	
	//Latency for one request
	tLatency=tAvg/(numThread * NUM_LOOP);
	
	//Total Bytes processed per second
	bps = (double) (totalBytes*1000000.0/tAvg);
	bps/=MEGABYTE;

	printf("%s, %d, %s, %.0f B, %.3f ms, %.3f ms, %.3lf MBPS \n",
		argv[1], numThread, argv[2], totalBytes, tLatency, tSD/(numThread * NUM_LOOP), bps);

	return 0;
}

