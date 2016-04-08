#!/bin/bash

#Single Node Hadoop Setup and starting the experiment

echo "Configuring Hadoop Files for pseudo-distributed mode..."
cd hadoop/etc/hadoop


#Creating HDFS Directory on RAID0
if [ ! -d /mnt/raid0/hdfs ]; then
	mkdir -p /mnt/raid0/hdfs
fi

#core-site.xml setup
echo "Configuring core-site.xml"
cp core-site.xml.template core-site.xml
sed -i 19,20d core-site.xml
echo "<configuration>
   <property>
		<name>fs.defaultFS</name>
		<value>hdfs://ec2-52-91-7-138.compute-1.amazonaws.com:9000</value>
	</property>
	<property>
		<name>hadoop.tmp.dir</name>
		<value>/mnt/raid0/hdfs</value>
	</property>
</configuration>" >> core-site.xml

#hdfs-site.xml setup
echo "Configuring hdfs-site.xml"
cp hdfs-site.xml.template hdfs-site.xml
sed -i 19,21d hdfs-site.xml
echo "<configuration>
    <property>
        <name>dfs.replication</name>
        <value>1</value>
    </property>
</configuration>" >> hdfs-site.xml

#mapred-site.xml setup
echo "Configuring mapred-site.xml"
cp mapred-site.xml.template mapred-site.xml
sed -i 19,21d mapred-site.xml
echo "<configuration>" >> mapred-site.xml
echo "   <property>" >> mapred-site.xml
echo "      <name>mapreduce.framework.name</name>" >> mapred-site.xml
echo "      <value>yarn</value>" >> mapred-site.xml
echo "   </property>" >> mapred-site.xml
echo "</configuration>" >> mapred-site.xml

#mapred-site.xml setup
echo "Configuring yarn-site.xml"
cp yarn-site.xml.template yarn-site.xml
sed -i 15,19d yarn-site.xml
echo "<configuration>" >> yarn-site.xml
echo "   <property>" >> yarn-site.xml
echo "      <name>yarn.nodemanager.aux-services</name>" >> yarn-site.xml
echo "      <value>mapreduce_shuffle</value>" >> yarn-site.xml
echo "   </property>" >> yarn-site.xml
echo "</configuration>" >> yarn-site.xml

cd ~
#Creating 10GB input file
echo "Generating input file..."
./64/gensort -a 107374183 /mnt/raid0/input

echo "Formating Namenode..."
hdfs namenode -format

echo "Starting HDFS..."
start-dfs.sh

echo "Copying input file to HDFS..."
hdfs dfs -put /mnt/raid0/input /

echo "File copied."
hdfs dfs -ls

echo "Starting YARN..."
start-yarn.sh

echo "Starting the application..."
hadoop jar ~/Terasort_yarn.jar /input output
