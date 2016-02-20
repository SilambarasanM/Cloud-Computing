##A. REQUIREMENTS
1. GCC Compiler (sudo apt-get install gcc)
2. Make (sudo apt-get install make)
3. Python (sudo apt-get install python)

##B. STEPS TO EXECUTE
Once all the requirements are setup and the files are in place, the following commands can be executed from the location where the source files are located.
###1. CPU Benchmark
python benchmarks.py cpu
###2. Memory Benchmark
python benchmarks.py memory
###3. Disk Benchmark
python benchmarks.py disk
###4. CPU Experiment
python benchmarks.py cpuExp
Note: The command python benchmarks.py all will execute all the benchmarks except the CPU experiment.
##C. ALTERNATE WAYS:
###1. CPU Benchmark
Compile: make cpu
Execute: ./cpu <FLOPS/IOPS> <NTHREADS>
Example: ./cpu FLOPS 1
./cpu IOPS 2
###2.Memory Benchmark
Compile: make memory
Execute: ./memory <SEQ|RAND> <1B|1KB|1MB> <NTHREADS>
Example: ./memory SEQ 1B 2
./memory RAND 1MB 1
###3. Disk Benchmark
Compile: make disk
Execute: ./disk <READ|WRITE> <SEQ|RAND> <1B|1KB|1MB> <NTHREADS>
Example: ./disk WRITE RAND 1KB 2
./disk READ SEQ 1MB 1
###4. CPU Experiment
Compile: make cpuExp
Execute: ./ReadCPU <FLOPS/IOPS>
Example: ./ReadCPU FLOPS
./ReadCPU IOPS
##D. OUTPUT
The binary executable files will be generated in the same directory as the source code. The output will be displayed on the Command Line Interface.
