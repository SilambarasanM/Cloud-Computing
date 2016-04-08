package main.scala
import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf


object TeraSort {
  def main(args: Array[String]){
    val sc = new SparkContext("local","terasort")
  
    val keyPairs = sc.textFile("hdfs://ec2-52-36-166-118.us-west-2.compute.amazonaws.com:9000/input").map(x => (x.slice(0,10),x.slice(10,99)))
    val action = keyPairs.sortByKey()
    action.map(k => k._1+"\t"+k._2).saveAsTextFile("hdfs://ec2-52-36-166-118.us-west-2.compute.amazonaws.com:9000/spark_output")
}
}