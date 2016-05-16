from threading import Thread
import boto.sqs
from boto.sqs.message import Message
import boto.dynamodb
import boto.s3
from boto.s3.connection import S3Connection
from boto.s3.key import Key
import time
import datetime
import argparse
import subprocess
import sys


#To execute a command or script
def execute(command):
	subP = subprocess.Popen(command, stdout=subprocess.PIPE)
	subP.wait()
	out, err = subP.communicate()
	
	#Printing Error Messages
	if err is not None:
		sys.stdout.write("Error: ",err)
	sys.stdout.write(out)

#Remote back-end Worker Class
class RemoteWorker(Thread):
	#Constructor
	def __init__(self, animotoFlag, threadID):
		Thread.__init__(self)
		self.animoto = animotoFlag
		self.threadID = threadID
		return
	
	#Entry point for Thread
	def run(self):
		#If Animoto Flag enabled
		if self.animoto:
			#Start Animoto Program
			self.startAnimoto()
		else:
			#Start Sleep Program
			self.startSleep()

	#Start Sleep Program
	def startSleep(self):
		#Connecting to AWS services
		aws_sqs = boto.sqs.connect_to_region('us-east-1')
		aws_dynamodb = boto.dynamodb.connect_to_region('us-east-1')
		taskQ = aws_sqs.get_queue('taskQ')
		resultQ = aws_sqs.get_queue('resultQ')
		tasksDB = aws_dynamodb.get_table('taskDB')
		
		waitFlag = False
		#Infinite Loop
		while True:
			#Receive in Batch
			resultSet = taskQ.get_messages(10)
			
			if not waitFlag:
				#Actual Start Time
				startTime = datetime.datetime.now()
				
			#Empty response
			if len(resultSet) == 0:
				waitFlag = True
				currentTime = datetime.datetime.now()
				runTime = (currentTime - startTime).seconds

				#If empty response received after 5 mins of start time
				if runTime > 300:
					break

			#Process each received message
			else:
			for result in resultSet:
					task = result.get_body()
					taskData = str(task).split('|')
					taskID = taskData[0]
					command = taskData[1]
					
					#Check for Duplicate
					if tasksDB.has_item(hash_key=taskID):
						taskQ.delete_message(result)
					else:
						#Put the TaskID into DynamoDB
						taskAttrib = {'Command': command}
						item = tasksDB.new_item(hash_key=taskID, attrs = taskAttrib)
						item.put()
						
						#Execute the sleep task
						time2Sleep = float(command.strip().split(' ')[1]) / 1000.0
						time.sleep(time2Sleep)
						
						#Delete the processed task from SQS
						taskQ.delete_message(result)
						
						# Sent result to SQS
						response = task + ' is done.'
						msg = Message()
						msg.set_body(response)
						resultQ.write(msg)
		return

	#To Upload the video to S3
	def upload2S3(self, taskID):
		#Connecting to AWS services
		aws_s3 = S3Connection()
		aws_sqs = boto.sqs.connect_to_region('us-east-1')
		resultQ = aws_sqs.get_queue('resultQ')
		bucket = aws_s3.get_bucket('smadanak')

		# Create a new Key
		k = Key(bucket)
		fileName = 'video'+taskID+'.mkv'
		k.key = fileName
		k.set_contents_from_filename(fileName)
		
		# Temporary URL valid for 300s
		url = k.generate_url(300)
		
		#Send URL in response
		msg = Message()
		msg.set_body(url)
		resultQ.write(msg)
		return

	def getImages(self):
		#Connecting to AWS services
		aws_sqs = boto.sqs.connect_to_region('us-east-1')
		aws_dynamodb = boto.dynamodb.connect_to_region('us-east-1')
		taskQ = aws_sqs.get_queue('taskQ')
		resultQ = aws_sqs.get_queue('resultQ')
		tasksDB = aws_dynamodb.get_table('taskDB')
		
		waitFlag = False
		while True:
			#Receive message one by one
			resultSet = taskQ.get_messages()
			if not waitFlag:
				#Actual Start Time
				startTime = datetime.datetime.now()
				
			#Empty response
			if len(resultSet) == 0:
				waitFlag = True
				currentTime = datetime.datetime.now()
				runTime = (currentTime - startTime).seconds

				#If empty response received after 5 mins of start time
				if runTime > 300:
					break

			# Process received messages
			else:
				for result in resultSet:
					task = result.get_body()
					taskData = str(task).split('|')
					taskID = taskData[0]
					command = str(task)

					#Check for Duplicate
					if tasksDB.has_item(hash_key=taskID):
						taskQ.delete_message(resultSet[0])
					else:
						#Put the TaskID into DynamoDB
						taskAttrib = {'Command': command}
						item = tasksDB.new_item(hash_key=taskID, attrs = taskAttrib)
						item.put()
						
						#Extract URLs and write to a file
						urls = command.split('|')
						with open('images'+taskID+'.txt', 'w') as f:
							for i in range(1,len(urls)):
								f.write(urls[i]+'\n')

						#Delete the processed task from SQS
						taskQ.delete_message(resultSet[0])
						
						#Execute script to create video
						execute(['./videoMaker.sh', taskID])
						#Upload the video to S3
						self.upload2S3(taskID)
		return		

	#Start Animoto Program	
	def startAnimoto(self):
		self.getImages()

if __name__ == '__main__':

	#Command Line Argument Parser
	parser = argparse.ArgumentParser()
	parser.add_argument('-a', '--animoto', help='Animoto', action='store_true')
	parser.add_argument('-t', metavar='<THREAD_COUNT>', type=int, help='the number of concurrent threads for a local worker')

	args = parser.parse_args()
	
	#Create Worker Threads
	remoteWorker = []
	for i in range(args.t):
		worker = RemoteWorker(args.animoto, i)
		worker.start()
		remoteWorker.append(worker)
	
	#Wait for the threads to complete
	for worker in remoteWorker:
		worker.join()

	#Delete all video files generated
	execute(['rm','*.mkv'])
