CC=gcc
MFLAG=-lm
CFLAGS=-lpthread -O0
TARGET=cpu memory disk ReadCPU

all: $(TARGET)

cpu: cpu.c
	$(CC) cpu.c -o $@ $(CFLAGS) $(MFLAG)

memory: memory.c 
	$(CC) memory.c -o $@ $(CFLAGS) $(MFLAG)

disk: disk.c 
	$(CC) disk.c -o $@ $(CFLAGS) $(MFLAG)

ReadCPU: ReadCPU.c 
	$(CC) ReadCPU.c -o $@ $(CFLAGS)

clean:
	rm -f *.dat $(TARGET)
