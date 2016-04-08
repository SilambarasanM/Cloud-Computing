package edu.cs553.pa2;

import java.io.*;
import java.util.ArrayList;
//Class to sort by block
public class SortByBlock implements Runnable{
	private final int id;
	private final long blockSize;
	private final long start;
	private RandomAccessFile  inFile;
	private final long end;
	private final long blockCount;
	private int count;
	
	/**
	 * Constructor to initialize variables
	 * @param ID - thread id
	 * @param startPos - start position in the input file
	 * @param endPos - end position in the input file
	 * @param bSize - available free memory for the thread
	 */
	public SortByBlock(int ID, long startPos, long endPos, long bSize){
		this.id = ID;
		this.start = startPos;
		this.end = endPos; //Need to fix the proper start end values
		
		try {
			inFile = new RandomAccessFile ("input.txt","r");
			inFile.seek(start);
		} catch (FileNotFoundException e ) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		this.blockSize = bSize;
		this.blockCount = bSize/100;
		this.count = 0;
	}
	
	/**
	 * Run method to be invoked by the thread
	 */
	public void run() {
		
		ArrayList<Record> records = new ArrayList<Record>();;
		long recsLoadedTotal = 0;
		
		try{
			//Loop until end position is reached
			for (String input=""; inFile.getFilePointer() < end ;){

				records = new ArrayList<Record>();
				long recsLoaded = 0;
				Record r = null;
				
				//Loop until the block limit is reached or until the end of the file
				while((recsLoaded <= blockCount) && inFile.getFilePointer() < end && (input = inFile.readLine()) != null){
					r = new Record(input);
					records.add(r);
					recsLoaded = recsLoaded + 1;
				}
				
				//Tracking total records loaded
				recsLoadedTotal+=recsLoaded;
				
				//Sort the block
				sort(records);
				String outputFile = "output"+Integer.toString(id) + "-" + Integer.toString(count);
				count++;

				//Write intermediate results
				write(records, outputFile);
				records.clear();				
			}
			
			inFile.close();
			System.out.println("Intermediate files generated in thread "+id+".");

			//Performing merge sort on the intermediate files
			SortByFile intSort = new SortByFile(count, false, id, blockSize);
			System.out.println("Sorting has begun in thread "+id+"...");

			//Merge sorting the files
			intSort.sort();
			System.out.println("Sorting completed in thread "+id+".");
		}
		catch(EOFException oef) {
			oef.printStackTrace();
        } catch (IOException e) {
			e.printStackTrace();
		} 		
	}

	/**
	 * To write the records to the file
	 * @param records - records to be written
	 * @param outputfile - output filename
	 */
	public void write(ArrayList<Record> records, String outputfile){
		File output = new File(outputfile);
		BufferedWriter bufferOut;
		try {		
				bufferOut = new BufferedWriter(new FileWriter(output));
				for(Record r : records){
					bufferOut.write(r.toString());
				}
				System.out.println("Created file "+output+".");
				bufferOut.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
	}
	
	/**
	 * Wrapper method for the Quick Sort method
	 * @param list record list to be sorted
	 */
	 public void sort(ArrayList<Record> list){
         
	        if (list == null || list.size() == 0){
	            return;
	        }
	        //Quick Sort
	        quickSort(list,0,list.size()-1);
	    }
	
	/**
	 * To perform quick sort
	 * @param list - list of records to be sorted
	 * @param low - lowest position of the list
	 * @param high - highest position of the list
	 */
	public void quickSort(ArrayList<Record> list, int low, int high) {
	         
	        // Pivot the median from the list
	        int pivotIdx = low + (high - low) / 2;
	        Record pivot = list.get(pivotIdx);
	        
	        int i = low, j = high;
	        while (i <= j) {
	            while (list.get(i).compareTo(pivot) < 0)
	                i++;
	            while (list.get(j).compareTo(pivot) > 0) 
	                j--;
	            if (i <= j) {
	            	//Swap
	            	list.set(j,list.set(i, list.get(j))) ;
	                i++; j--;
	            }
	        }
	        //Recursive Sorting
	        if (low < j)
	            quickSort(list, low, j);
	        if (i < high)
	            quickSort(list, i, high);
	    }
	

}
