package edu.cs553.pa2;

public class Record  implements Comparable<Record>
{
	String key;
	String value;
	
	/**
	 * Record single parameter constructor
	 * @param s - line from the input file
	 */
	public Record(String s)
	{
		this.key = s.substring(0, 11);
		this.value = s.substring(11, s.length());
	}
	
	/**
	 * To get the key for this record
	 * @return - returns the key
	 */
	public String getKey()
	{
		return key;
	}
	
	/**
	 * To convert the record to a string
	 */
	public String toString()
	{
		return key+value+"\r\n";
	}
	
	/**
	 * Establish a Comparable method to compare the records
	 */
	@Override
	public int compareTo(Record r) {
		return this.key.compareTo(r.key);
	}
}
