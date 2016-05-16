from Queue import Queue
from threading import Thread
import argparse
import time
import boto.sqs
from boto.sqs.message import Message
import boto.dynamodb

#Global Queues
taskQ = Queue()
resultQ = Queue()

#The Client Class
class Client(object):
    #Constructor
	def __init__(self, workloadFile):
		self.workloadFile = workloadFile
		self.tasks = self.loadTasks()
		self.taskID = []
		self.results = []
		return
    
    #Load tasks from WorkloadFile
	def loadTasks(self):
		with open(self.workloadFile,'r') as f:
			tasks = []
			for line in f:
				tasks.append(line.replace('\n',''))
		return tasks
    
    #Sending Tasks to in-memory taskQ
	def sendTasks(self):
		#Accessing global taskQ
		global taskQ
		
		#Creating Message dictionary for in-memory queue
		for taskId in range(len(self.tasks)):
			msg = {}
			msg['ID'] = taskId
			msg['Task'] = self.tasks[taskId]
			self.taskID.append(taskId)
			taskQ.put(msg)
		return
    
    #Receiving Results from in-memory resultQ
	def getResults(self):
		#Accessing global resultQ
		global resultsQ
        #Infinite Loop
		while True:
			#Stop when all tasks are completed
			if len(self.taskID) == 0:
				break
			else:
				response = resultQ.get()
				#Remove the task from tracking list after receiving response
				if response['ID'] in self.taskID:
					if response['Status'] == 0:
						self.taskID.remove(response['ID'])
		return

	#Send sleep tasks to SQS taskQ
	def sendTasks2SQS(self):
		#Connecting to AWS services
		aws_sqs = boto.sqs.connect_to_region('us-east-1')
		taskQ = aws_sqs.get_queue('taskQ')
		
		#Send all the tasks read from workload file
		for taskID in range(len(self.tasks)):
			task = self.tasks[taskID]
			msg = Message()
			msg.set_body(str(taskID)+'|'+task)
			taskQ.write(msg)
		return
	
	#Send image URLs to SQS taskQ
	def sendURLs2SQS(self):
		#Connecting to AWS services
		aws_sqs = boto.sqs.connect_to_region('us-east-1')
		taskQ = aws_sqs.get_queue('taskQ')
				
		for taskID in range(len(self.tasks)):
			urls = []
			with open(self.tasks[taskID],'r') as f:
				
				for line in f:
					urls.append(line.replace('\n',''))
			task = '|'.join(urls)
			msg = Message()
			msg.set_body(str(taskID)+'|'+task)
			taskQ.write(msg)
		return
		
	#Receive responses from SQS resultQ
	def getResultsFromSQS(self):
		#Connecting to AWS services
		aws_sqs = boto.sqs.connect_to_region('us-east-1')
		resultQ = aws_sqs.get_queue('resultQ')

		while len(self.results) < len(self.tasks):
			response = resultQ.get_messages(10)
			for result in response:
				if result.get_body() not in self.results:
					self.results.append(result.get_body())
				resultQ.delete_message(result)
		return
		
#The Local back-end Worker Class
class LocalWorker(Thread):
	#Constructor
	def __init__(self, id):
		Thread.__init__(self)
		self.threadId = id
		return
		
	#Entry point for thread
	def run(self):
		global resultQ
		global taskQ

		#Execute tasks from taskQ and send response
		while not taskQ.empty():
			task = taskQ.get()
			time2Sleep = int(task['Task'].split(' ')[1].strip().replace('\n','')) / 1000.0
			time.sleep(time2Sleep)
			result = str(task['ID']) + ' status reported.'
			task['Status'] = 0
			resultQ.put(task)
		return

if __name__ == '__main__':    

	#Command Line Argument Parser
	parser = argparse.ArgumentParser()
	
	group = parser.add_mutually_exclusive_group(required=True)
	group.add_argument('-l', '--local', help='local worker version', action='store_true')
	group.add_argument('-r', '--remote', help='remote worker version', action='store_true')
	
	parser.add_argument('-a', '--animoto', help='animoto version', action='store_true')
	parser.add_argument('-t', metavar='<THREAD_COUNT>', type=int, help='the number of concurrent threads for a local worker')
	parser.add_argument('-w', metavar='<WORKLOAD_FILE>', type=str, required=True, help='the workload file')

	args = parser.parse_args()
	
	#Validation
	if args.local:
		if args.t==None:
			print 'error: argument -t is required with -l'
			exit()
	
	#Initializing Client
	client = Client(args.w)	
	
	#Start Timer
	startTime = time.time()

	#If Local Flag is enabled
	if args.local:
		#Create Local Worker Threads
		localWorkers = []
		for i in range(args.t):
			worker = LocalWorker(i)
			worker.start()
			localWorkers.append(worker)
		
		#Send tasks to in-memory Queue
		client.sendTasks()
		
		#Get results from in-memory Queue
		client.getResults()
		
		#Wait for Workers to complete
		for worker in localWorkers:
			worker.join()
	
	#If Remote Flag is enabled
	elif args.remote:
		#If Animoto Flag is enabled
		if args.animoto:
			#Send URLs to SQS
			client.sendURLs2SQS()
		else:
			#Send sleep tasks to SQS
			client.sendTasks2SQS()
			
		#Get results from SQS queue
		client.getResultsFromSQS()
		
	#Stop Timer
	endTime = time.time()
	elapsedTime = 1000 * (endTime - startTime)
	print 'Elapsed: {} ms' .format(elapsedTime)