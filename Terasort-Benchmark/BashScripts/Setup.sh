#!/bin/bash

#Initial Installation 
echo "Running Update..."
sudo yum update

echo "Downloading JAVA..."
wget --no-check-certificate --no-cookies --header "Cookie: oraclelicense=accept-securebackup-cookie" http://download.oracle.com/otn-pub/java/jdk/8u73-b02/jdk-8u73-linux-x64.tar.gz

echo "Installing Java..."
sudo tar -xvzf jdk-8u73-linux-x64.tar.gz
ln -s jdk1.8.0_73 java

echo "Downloading Hadoop..."
wget http://mirror.reverse.net/pub/apache/hadoop/common/hadoop-2.7.2/hadoop-2.7.2.tar.gz

echo "Installing Hadoop..."
sudo tar -xvzf hadoop-2.7.2.tar.gz
ln -s hadoop-2.7.2 hadoop
sudo chown -R ec2-user hadoop-2.7.2
sudo chgrp -R ec2-user hadoop-2.7.2

echo "Downloading SPARK..."
wget http://d3kbcqa49mib13.cloudfront.net/spark-1.6.0-bin-hadoop2.6.tgz

echo "Insatlling SPARK..."
sudo tar -xvzf spark-1.6.0-bin-hadoop2.6.tgz
ln -s spark-1.6.0-bin-hadoop2.6 spark

echo "Downloading SCALA..."
wget http://downloads.lightbend.com/scala/2.11.8/scala-2.11.8.tgz

echo -e "Insatlling SCALA..."
sudo tar -xvzf scala-2.11.8.tgz
ln -s scala-2.11.8 scala 
sudo chown -R ec2-user scala-2.11.8
sudo chgrp -R ec2-user scala-2.11.8

echo "Downloading Gensort..."
wget http://www.ordinal.com/try.cgi/gensort-linux-1.5.tar.gz

echo "Extracting Gensort..."
sudo tar -xvzf gensort-linux-1.5.tar.gz

rm *gz 
echo "Exporting PATH variables..."
sudo chmod 777 ~/.bashrc
echo 'export PATH=~/hadoop/bin:~/hadoop/sbin:~/jdk/bin:~/scala/bin:~/spark/bin:$PATH' >> ~/.bashrc 
echo 'export HADOOP_HOME=~/hadoop' >> ~/.bashrc
echo 'export JAVA_HOME=~/jdk' >> ~/.bashrc
echo 'export SCALA_HOME=~/scala' >> ~/.bashrc
echo 'export SPARK_HOME=~/spark' >> ~/.bashrc

source .bashrc

#Taking backup of Default files for later scripting
cd ~/hadoop/etc/hadoop
cp core-site.xml core-site.xml.template
cp hdfs-site.xml hdfs-site.xml.template
cp yarn-site.xml yarn-site.xml.template

#Setting up Password-less SSH
echo "Setting up Password-less SSH"
ssh-keygen -f ~/.ssh/id_rsa -t rsa -N ''
cat .ssh/id_rsa.pub >> .ssh/authorized_keys
chmod 600 .ssh/authorized_keys
echo "StrictHostKeyChecking=no" >> ~/.ssh/config

./Setup_RAID0.sh