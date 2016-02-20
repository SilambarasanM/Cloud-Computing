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
#define NUM_LOOP_B 9900000
#define NUM_LOOP_KB 99000
#define NUM_LOOP_MB 200
#define CHAR_SIZE sizeof(char)

int numBytes, *randNumbers, NUM_LOOP;
char* randData;

//Generates random character data
void genChar(char* variable, int size)
{
	int i;

	for(i = 0; i < size; i++)
		memset(&variable[i], (char) (32 + rand() % 95), CHAR_SIZE);

	return ;
}

//Perform Sequential Write
void *doSEQWrite(void *arg) 
{
	int i;
	FILE* data = (FILE*) arg;
	
	for (i = 0; i < NUM_LOOP; i++) 
		fwrite(randData, CHAR_SIZE, numBytes, data);
	
	pthread_exit(NULL);
}

//Perform Sequential Read
void *doSEQRead(void *arg)
{
	int i;
	FILE* data = (FILE*)arg;
	char* read = (char*) malloc(numBytes*CHAR_SIZE);

	for (i = 0; i < NUM_LOOP; i++) 
		fread(read,CHAR_SIZE, numBytes, data);

	free(read);
	pthread_exit(NULL);
}

//Perform Random Write
void *doRANDWrite(void *arg)
{
	int i;
	FILE* data = (FILE*)arg;	

	for (i = 0; i < NUM_LOOP; i++)
	{
		fseek(data, *(randNumbers+i)*numBytes, SEEK_SET);
		fwrite(randData, CHAR_SIZE, numBytes, data);
	}

	pthread_exit(NULL);
}

//Perform Random Read
void *doRANDRead(void *arg)
{
	int i;
	FILE* data = (FILE*)arg;
	char* read = malloc(numBytes*CHAR_SIZE);


	for (i = 0; i < NUM_LOOP; i++)
	{
		fseek(data, *(randNumbers+i)*numBytes, SEEK_SET);
		fread(read,CHAR_SIZE, numBytes, data); 
	}

	free(read);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) 
{
	int i, j, numThread, bytesPerFile, temp;
	struct timeval tStart, tStop;
	double tElapsed[REPEAT], tTotal, tAvg, tVar, tSD, totalBytes, bps, tLatency, tRandTime, tLoop;

	if(argc < 5)
	{
		printf("Insufficient Arguments Error.\n");
		printf("Usage: %s <READ|WRITE> <SEQ|RAND> <1B|1KB|1MB> <NumberOfThreads>\n", argv[0]);
		return -1;
	}
	
	numThread = atoi(argv[4]);

	//Number of bytes given in CLI argument
	if(strcmp(argv[3],"1B") == 0)
	{
		numBytes = BYTE;
		NUM_LOOP = NUM_LOOP_B;
	}
	else if(strcmp(argv[3],"1KB") == 0)
	{
		numBytes = KILOBYTE;
		NUM_LOOP = NUM_LOOP_KB;
	}
	else if(strcmp(argv[3],"1MB") == 0)
	{
		numBytes = MEGABYTE;
		NUM_LOOP = NUM_LOOP_MB;
	}

	//Bytes per File
	bytesPerFile = NUM_LOOP * numBytes;

	//Total number of bytes processed
	totalBytes = numThread * bytesPerFile;

	pthread_t tIDRead[numThread], tIDWrite[numThread];

	FILE *readData[numThread], *writeData[numThread];

	//Filename construction
	char fileName[50];
	strcpy(fileName, argv[1]);
	strcat(fileName, "data");

	for(i = 0; i < numThread; i++)
	{
		if(strcmp(argv[1], "READ") == 0)
		{	//Filename for each thread
			char fn[50]; strcpy(fn, fileName);
			char str[3] = " "; sprintf(str, "%d", i);
			strcat(fn, str); strcat(fn, ".dat");

			//Opening file to be used for read operation
			readData[i] = fopen(fn, "w+");

			//Random Data generation for the file to be read
			randData = (char*) malloc(bytesPerFile * CHAR_SIZE);
			genChar(randData, bytesPerFile);

			fwrite(randData, CHAR_SIZE, bytesPerFile, readData[i]);
			
			free(randData);			
			rewind(readData[i]);
		}
		else if(strcmp(argv[1], "WRITE") == 0)
		{	//Filename for each thread
			char fn[50]; strcpy(fn, fileName);
			char str[3] = " "; sprintf(str, "%d", i);
			strcat(fn, str); strcat(fn, ".dat");

			//Opening file to be used for write operation
			writeData[i] = fopen(fn, "w+");
		}
	}

	//Generating random data to be written to file
	if(strcmp(argv[1], "WRITE") == 0)
	{
		randData = (char*) malloc(numBytes * CHAR_SIZE);
		genChar(randData, numBytes);
	}

	//Random numbers to be used for random access
	randNumbers = (int*) malloc(NUM_LOOP * INT_SIZE);
	for(j = 0; j < NUM_LOOP; j++) *(randNumbers+j) = rand() % NUM_LOOP;
	
	//Time taken for accessing random number pointer and multiplication within loop
	gettimeofday(&tStart, NULL);
	for (i = 0; i < NUM_LOOP; i++)
		*(randNumbers+i)*numBytes;
	gettimeofday(&tStop, NULL);

	tRandTime = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
	
	//Time taken for an empty loop
	gettimeofday(&tStart, NULL);	
	for (i = 0; i < NUM_LOOP; i++){}
	gettimeofday(&tStop, NULL);
	
	tLoop = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
	
	//Branching to handle each case
	if(strcmp(argv[2], "SEQ") == 0)
	{
		if(strcmp(argv[1], "READ") == 0)
		{
			for (j = 0; j < REPEAT; j++)
			{
				gettimeofday(&tStart, NULL);

				//pthread creation
				for(i = 0; i < numThread; i++) pthread_create(&tIDRead[i], NULL, doSEQRead, (void*)readData[i]); 

				//pthread join
				for (i = 0; i < numThread; i++) pthread_join(tIDRead[i], NULL); 

				gettimeofday(&tStop, NULL);

				tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
				tElapsed[j]-=tLoop;	//Reducing looping time
			}
		}
		else if(strcmp(argv[1], "WRITE") == 0)
		{
			for (j = 0; j < REPEAT; j++)
			{
				gettimeofday(&tStart, NULL); 

				//pthread creation
				for(i = 0; i < numThread; i++) pthread_create(&tIDWrite[i], NULL, doSEQWrite, (void*)writeData[i]);

				//pthread join
				for (i = 0; i < numThread; i++)	pthread_join(tIDWrite[i], NULL); 

				gettimeofday(&tStop, NULL);

				tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
				tElapsed[j]-=tLoop;	//Reducing looping time
			}
		}
	}
	else if(strcmp(argv[2], "RAND") == 0)
	{
		if(strcmp(argv[1], "READ") == 0)
		{
			for (j = 0; j < REPEAT; j++)
			{
				gettimeofday(&tStart, NULL);

				//pthread creation
				for(i = 0; i < numThread; i++) pthread_create(&tIDRead[i], NULL, doRANDRead, (void*)readData[i]); 

				//pthread join
				for (i = 0; i < numThread; i++) pthread_join(tIDRead[i], NULL); 

				gettimeofday(&tStop, NULL);

				tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec)  * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
				tElapsed[j]-=tRandTime;	//Reducing random pointer access and multiplication time
			}
		}
		else if(strcmp(argv[1], "WRITE") == 0)
		{
			for (j = 0; j < REPEAT; j++)
			{
				gettimeofday(&tStart, NULL);

				//pthread creation
				for(i = 0; i < numThread; i++) pthread_create(&tIDWrite[i], NULL, doRANDWrite, (void*)writeData[i]);

				//pthread join
				for (i = 0; i < numThread; i++)	pthread_join(tIDWrite[i], NULL); 

				gettimeofday(&tStop, NULL);

				tElapsed[j] = (double) (tStop.tv_sec - tStart.tv_sec) * 1000000.0 + (double)(tStop.tv_usec - tStart.tv_usec);
				tElapsed[j]-=tRandTime;	//Reducing random pointer access and multiplication time
			}
		}
	}

	//Closing files
	for(i = 0; i < numThread; i++)
	{
		if(strcmp(argv[1], "READ") == 0) fclose(readData[i]);
		else if(strcmp(argv[1], "WRITE") == 0) fclose(writeData[i]);
	}

	//Deallocation of memory used for pointers
	if(strcmp(argv[1], "WRITE") == 0) free(randData);
	
	free(randNumbers);

	//Adding the time samples
	for(i = 0; i < REPEAT; i++) tTotal+=tElapsed[i]; 

	//Calculating the average time
	tAvg= tTotal/REPEAT;

	//Calculating the Variance and Standard Deviation
	for(i = 0; i < REPEAT; i++) tVar+=pow(((tElapsed[i])-tAvg),2);
	
	tVar/=REPEAT; tSD = sqrt(tVar);

	//Latency for one request
	tLatency=tAvg/(numThread * NUM_LOOP);
	
	//Total Bytes processed per second
	bps = (double) (totalBytes*1000000.0/tAvg);
	bps/=MEGABYTE;

	printf("%s, %s, %d, %s, %.0f B, %.3f ms, %.3f ms, %.3lf MBPS \n",
		argv[1], argv[2], numThread, argv[3], totalBytes, tLatency, tSD/(numThread * NUM_LOOP), bps);
	
	return 0;
}

