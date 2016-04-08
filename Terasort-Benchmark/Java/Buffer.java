package edu.cs553.pa2;

import java.io.*;

//Class to access the intermediate files
public class Buffer {
	public Record record;
    public File outputFile;
    public BufferedReader bufferOut;
    
    /**
     * Constructor for Buffer Class
     * @param file - the file to be buffered
     */
    public Buffer(String file) {
        try{
        	outputFile = new File(file);
        	bufferOut = new BufferedReader(new FileReader(outputFile));

        	//Reading the first record from the file 
        	update();
        }
        catch(IOException e){
        	e.printStackTrace();
        }
    }
    
    /**
     * To update the existing record
     * @return returns false if it fails
     */
    public boolean update(){
    	String line = "";
    	
        try {
        	if((line = bufferOut.readLine()) != null){
        		record = new Record(line);
        		return true;
        	}
        } catch(EOFException e) {
        	e.printStackTrace();
        } catch (IOException e) {
        	e.printStackTrace();
        }
        //By default setting record to null
        record = null;
        return false;
    }
    
    /**
     * To get the current record in buffer
     * @return returns the current record
     */
    public Record getRecord() {return record;}
    
    /**
     * To get the file used in this buffer
     * @return returns the file name which is buffered
     */
    public String getFileName() {return outputFile.getName();}
    
    /**
     * To close and delete the intermediate file.
     */
    public void close() {
    	try {
			bufferOut.close();
			outputFile.delete();
			System.out.println("Deleted file "+outputFile.getName()+".");
		} catch (IOException e) {
			e.printStackTrace();
		}
    }

    /**
     * To remove and update the current record
     * @return returns the removed record
     */
    public Record remove() {
    	Record r = getRecord();
    	update();
      return r;
    }
     
     
 
}