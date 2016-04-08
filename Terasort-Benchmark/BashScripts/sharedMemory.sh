#!/bin/bash
clear
#Getting Input
if [ $# -ne 2 ] ; then
	echo -n "Usage: $0 <Filesize in bytes> <ThreadCount>"
	echo
	exit 1
fi

if [ "$1" == "1GB" ]; then
	let fileSize=10737419
else
	if [ "$1" == "10GB" ]; then
		let fileSize=107374183
	else
		echo "Error: Unrecognized filesize $1"
		exit 1
	fi
fi

#Removing existing files
if [ -f output.txt ]; then
	rm output.txt
fi

let numThread=$2

echo "[`date "+%m/%d/%y %H:%M:%S"`] Starting experiment for $((fileSize*100)) with $numThread threads..."
echo "[`date "+%m/%d/%y %H:%M:%S"`] Generating gensort data..."

#If old input file exists keep it as it is and skip file generation else generate new file
if [ -f input.txt ]; then
	touch input.txt
	echo "[`date "+%m/%d/%y %H:%M:%S"`] Input file input.txt generated"
	ls -ls input.txt | awk '{print $10,$6,$7,$8,$9}'
else
	./64/gensort -a $fileSize input.txt
	echo "[`date "+%m/%d/%y %H:%M:%S"`] Input file input.txt generated"
	ls -ls input.txt | awk '{print $10,$6,$7,$8,$9}'
fi

#Run External Sort JAVA program
echo "[`date "+%m/%d/%y %H:%M:%S"`] Executing External Sort..."
java -Xmx1g -jar ExternalSort.jar $numThread
echo "[`date "+%m/%d/%y %H:%M:%S"`] Sorting Completed."

#Printing Results
if [ -f output.txt ]; then
	ls -ls output.txt | awk '{print $10,$6,$7,$8,$9}'
	echo "head -10 output.txt"
	head -10 output.txt
	echo "tail -10 output.txt"
	tail -10 output.txt
	echo "Running Valsort on output file..."
	 ./64/valsort output.txt
else
	echo "Error: output.txt file was not generated."
fi
