#!/usr/bin/python2.7
"""
Usage: python benchmarks.py [all] [cpu] [memory] [disk] [cpuExp]
"""
import subprocess
import sys

#'''To execute the input command '''
def execute(command):
  subP = subprocess.Popen(command, stdout=subprocess.PIPE)
  subP.wait()
  out, err = subP.communicate()
  #sys.stdout.write(popen.stdout.read())
  if err is not None:
    sys.stdout.write("Error: ",err)
  sys.stdout.write(out)

#'''To execute a make command'''
def make(arg):
  execute(['make', arg])

#To invoke cpu benchmark
def cpu(loop):
  operations = ['FLOPS','IOPS']
  threads = ['1','2','4']
  make('cpu')
  cpuStr = './cpu'
  runsArgs = []

  print "*******************************************************************"
  print "                          CPU BENCHMARK                            "
  print "*******************************************************************"
  print "OP_TYPE, N_THREAD, N_OPERATIONS, T_TAKEN_AVG, T_TAKEN_SD, SPEED"

  for i in range(loop):
    for op in operations:
      for th in threads:
        runArgs = [cpuStr,op,th]
        execute(runArgs)

#To invoke cpu experiment
def cpuExp(loop):
  operations = ['FLOPS','IOPS']
  make('ReadCPU')
  cpuStr = './ReadCPU'
  runsArgs = []

  for i in range(loop):
    for op in operations:
      runArgs = [cpuStr,op]
      execute(runArgs)

#To invoke memory benchmark
def memory(loop):
  operations = ['SEQ', 'RAND']
  threads = ['1','2']
  size = ['1B','1KB','1MB']
  make('memory')
  cpuStr = './memory'
  runsArgs = []

  print "******************************************************************************************"
  print "                                    MEMORY BENCHMARK                                      "
  print "******************************************************************************************"
  print "ACCESS_MODE, NUM_THREAD, BLOCK_SIZE, TOTAL_BYTES, T_LATENCY_AVG, T_LATENCY_SD, THROUGHPUT"

  for i in range(loop):
    for op in operations:
      for sz in size:
        for th in threads:
          runArgs = [cpuStr, op, sz, th]
          execute(runArgs)

#To invoke disk benchmark
def disk(loop):
  operations = ['READ','WRITE']
  access = ['SEQ','RAND']
  threads = ['1','2']
  size = ['1B','1KB','1MB']
  make('disk')
  cpuStr = './disk'
  runsArgs = []
  print "*****************************************************************************************************"
  print "                                            DISK BENCHMARK                                           "
  print "*****************************************************************************************************"
  print "OPERATION, ACCESS_MODE, NUM_THREAD, BLOCK_SIZE, TOTAL_BYTES, T_LATENCY_AVG, T_LATENCY_SD, THROUGHPUT"
  for i in range(loop):
    for op in operations:
      for ac in access:
        for sz in size:
          for th in threads:
            runArgs = [cpuStr, op, ac, sz, th]
            execute(runArgs)
  
  
#Main  section form where the program begins
if __name__ == "__main__":
  if len(sys.argv) == 1:
    sys.stderr.write(__doc__)
    exit(1)

  #Default number of times the experiments to be run
  loop = 1
  for arg in sys.argv:
    if arg == 'cpu' or arg == 'all' :
      cpu(loop)
    if arg == 'memory' or arg == 'all' :
      memory(loop)
    if arg == 'disk' or arg == 'all' :
      disk(loop)
    if arg == 'cpuExp' :
      cpuExp(loop)
    make('clean')
