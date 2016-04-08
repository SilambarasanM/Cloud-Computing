package edu.cs553.pa2;
import java.io.File;

//The Main Class that runs the external sorting
public class ExternalSort{

	public static void main(String args[]) {
		
		//Getting thread count from CLI
		int numThread = Integer.parseInt(args[0]);
		File input = new File("input.txt");
		long fileSize = input.length();
		long rowCount = fileSize/100;
		long[] chunkStart = new long[numThread+1];
		System.out.println("FileSize:"+fileSize);
		
		//Calculating Chunk Size for each thread
		for (int i = 0; i<numThread; i++)
		{
			chunkStart[i] = rowCount*i/numThread;
			chunkStart[i] *=100;
			//System.out.println("ChunkStart:"+chunkStart[i]);
		}
		chunkStart[numThread] = fileSize;
	    
		Thread[] t = new Thread[numThread];
		
		//Starting clock
	    long startTime = System.nanoTime();
	    long bSize=0;

	    //Getting block size for each thread
	    bSize = getBlockSize(numThread, fileSize);
	    
	    //Starting Threads
	    for (int i = 0; i<numThread; i++){
		    t[i] = new Thread(new SortByBlock(i, chunkStart[i], chunkStart[i+1], bSize));
		    System.out.println("Starting Thread "+i+"...");
		    t[i].start();
	    }   
	    
	    //Joining Threads
	    for (int i = 0; i<numThread; i++){
	    	try {
	    		t[i].join();
	    		System.out.println("Thread "+i+" completed.");
	    		input.delete();
	    		System.out.println("Deleted file "+input.getName()+".");
	    	} catch (InterruptedException e) {
	    		e.printStackTrace();
	    	}
	    }
	    
	    //Getting new block size after threads complete
	    bSize = getBlockSize(numThread, fileSize); 
	    
	    System.out.println("Starting the final sort...");
	    SortByFile extSort = new SortByFile(numThread, true, -1, bSize);

	    //Sorting by File
	    extSort.sort();
	    System.out.println("Sorting Completed.");

	    //Stopping Clock
	    long stopTime = System.nanoTime();
	    double timeElapsed = (double)(stopTime-startTime)/1000000000.0;
	    double fileSizeMB = (double)fileSize/(double)(1024*1024);
	    double throughput = fileSizeMB/timeElapsed;
	    
	    System.out.println("File Size\t:  "+fileSizeMB+" MB");
	    System.out.println("Time Elapsed\t:  "+timeElapsed+" s");
	    System.out.println("Throughput\t:  "+throughput+" MBPS");
		System.exit(0);
		
   }
	
	/**
	 * To calculate the block size from the free memory available for the JVM
	 * @param numThread - number of threads that share the free memory
	 * @param chunkSize - size of the file chunk
	 * @return returns the block size per thread
	 */
	public static long getBlockSize(int numThread, long chunkSize){
		//Invoking Garbage Collector
		System.gc();

		//Getting Available Free Memory Size
		long freeSize = Runtime.getRuntime().freeMemory();
		System.out.println("Free Memory: "+ freeSize);
		if (chunkSize<freeSize)
			return chunkSize/numThread;
		else
			return freeSize/numThread;
	}
	
}