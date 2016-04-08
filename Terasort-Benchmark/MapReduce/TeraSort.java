import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;

public class TeraSort {

	public static class Map extends MapReduceBase implements Mapper<Object, Text, Text, Text>{
		private Text outKey;
		private Text outValue;
		
	@Override
	public void map(Object inKey, Text inValue, OutputCollector<Text, Text> output, Reporter arg3) throws IOException {
		String file = inValue.toString();
    	String[] lines = file.split("\n");
    	for(String line: lines){
    		outKey = new Text (line.substring(0,11));
    		outValue = new Text (line.substring(11));
    		output.collect(outKey, outValue);
    	}
	}
  }

  public static class Reduce
       extends MapReduceBase implements Reducer<Text,Text,Text,Text> {

	@Override
	public void reduce(Text inKey, Iterator<Text> inValues, OutputCollector<Text, Text> output, Reporter arg3)
			throws IOException {
		while(inValues.hasNext()) 	      
	      output.collect(inKey, inValues.next());
		
	}
  }

  public static void main(String[] args) throws Exception {
	  JobConf jConf = new JobConf("TeraSort.class");
	  jConf.setJobName("TerSort");
	  
	  jConf.setOutputKeyClass(Text.class);
	  jConf.setOutputValueClass(Text.class);
	  
	  jConf.setMapperClass(Map.class);
	  jConf.setReducerClass(Reduce.class);
	  
	  jConf.setInputFormat(TextInputFormat.class);
	  jConf.setOutputFormat(TextOutputFormat.class);
	  FileInputFormat.setInputPaths(jConf, new Path(args[0]));
	  FileOutputFormat.setOutputPath(jConf, new Path(args[1]));
	  
      JobClient.runJob(jConf);
  }
}