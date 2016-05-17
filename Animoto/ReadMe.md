### A. REQUIREMENTS
1. Python 2.7.10
2. ffmpeg
3. Pssh

### B. SETUP

1.	~/.aws/credentials file should have the Access Key Id and Secret Access Key in the below mentioned format before running any of the above mentioned program using SQS, S3 and DynamoDB.

2.	The program assumes taskQ and resultQ SQS queues are already available. The DynamoDB table taskDB is already created with a partition key. Also, an S3 bucket smadanak is already created.

3.	The provisioned Read and Write capacities for the taskDB is kept at 100 and 50 respectively.

### C. STEPS TO EXECUTE

Once all the requirements are setup and the files are in place, the following commands can be executed from the location where the source files are located.
1. Local Worker:

Client and Local Worker:	python Client --local -t <THREAD_COUNT> -w <WORKLOAD_FILE>
2. Remote Worker:

Client:	 		python Client --remote -w <WORKLOAD_FILE>
Remote Worker:	 python RemoteWorker.py -t <THREAD_COUNT>
3. Animoto:

Client:			python Client --remote -a -w <WORKLOAD_FILE>
Remote Worker:	python RemoteWorker.py -a -t <THREAD_COUNT>

### D. OUTPUT

The binary executable files will be generated in the same directory as the source code. The output will be displayed on the Command Line Interface for the time taken. In case of Animoto, the video files will be loaded to the S3 bucket.
