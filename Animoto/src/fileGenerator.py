#To Generate Workload file

if __name__ == '__main__':  

	worker = [1, 2, 4, 8, 16]
	sleep = [10, 1000, 10000]
	tasks = [1000, 100, 10]
	
	for workerCount in worker:
		for sleepTask in range(len(sleep)):
			f = open(str(workerCount)+'worker_'+str(sleep[sleepTask])+'ms.txt', "w")
			for loopCounter in range(tasks[sleepTask]*workerCount):
					f.write('sleep '+str(sleep[sleepTask])+"\n")
			f.close()