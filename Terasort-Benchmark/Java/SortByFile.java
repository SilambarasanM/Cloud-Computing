package edu.cs553.pa2;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Vector;

//To perform sorting by file
public class SortByFile {
	Vector<Buffer> outputFiles;
	public BufferedWriter bufferOut;
	public long bufferLen;
	public String fileName;
	public ArrayList<Record> outRec;
	
	/**
	 * Constructor to initialize the fields
	 * @param count - the file count
	 * @param mode - to identify the calling thread(true if main)
	 * @param id - The identifier for the thread
	 * @param blockSize - free memory that can be utilized
	 */
	public SortByFile(int count, boolean mode, int id, long blockSize) {
		outputFiles = new Vector<Buffer>();
		
		try {
			if (mode) {//Main Thread
				fileName = "output.txt";
				bufferOut = new BufferedWriter(new FileWriter(fileName));
				
			}
			else{	//MultiThread
				fileName = "output"+Integer.toString(id);
				bufferOut = new BufferedWriter(new FileWriter(fileName));		
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		outRec = new ArrayList<Record>();
		bufferLen = (blockSize/100)/(count+1);
		
		//Preparing the files to be processed
		for(int i = 0; i<count; i++){
			String fileName;
			if (mode)
					fileName = "output"+Integer.toString(i);
			else
					fileName = "output"+Integer.toString(id)+"-"+Integer.toString(i);
			outputFiles.add(new Buffer(fileName));
		}
	}
	
	/**
	 * To sort the files
	 */
	public void sort(){
		boolean breakFlag = false;
		
		while(outputFiles.size()!=0){

			Record min = outputFiles.get(0).getRecord();
			int minIdx = 0;
			
			//Find the minimum record and write to output
			for(int i=0; i<outputFiles.size(); i++){
				
				Record tmp = outputFiles.get(i).getRecord();
				
				if (tmp!= null && (min.compareTo(tmp) > 0)){
					min = tmp;
					minIdx = i;
				}
				//If buffer is null, close the file
				else if(tmp==null){
					outputFiles.get(i).close();
					outputFiles.remove(i);
					breakFlag = true;
					break;
				}
			}
			//Write the minimum record found
			if(outputFiles.size()!=0 && !breakFlag){
				Record d = outputFiles.get(minIdx).remove();
				write(d);
			}
			
			breakFlag =false;
		}
		//Final write for remaining records
		if(outRec.size()!=0){
			System.out.println("Writing "+outRec.size()+" record(s)..");
			write();
		}
		
		try {
			if (outRec.size()!=0)
				write();
			System.out.println("Created file "+fileName+".");
			bufferOut.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * To perform the final write
	 */
	public void write(){
		try {
			int size = outRec.size();
			for(int i =0; i<size;i++)
			bufferOut.write(outRec.remove(0).toString());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * To write the records into the file
	 * @param r
	 */
	public void write(Record r){
		//Buffer the records until buffer limit is reached
		outRec.add(r);
		
		if (outRec.size() >= bufferLen){
			try {
				System.out.println("Writing "+outRec.size()+" record(s)..");
				int size = outRec.size();

				//Write only when the buffer limit is reached
				for(int i =0; i<size;i++)
					bufferOut.write(outRec.remove(0).toString());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
