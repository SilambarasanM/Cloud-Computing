#!/bin/bash

if [ ! -d /mnt/raid0/hdfs/namenode ]; then
	mkdir -p /mnt/raid0/hdfs/namenode
fi

#Public IP
#curl http://169.254.169.254/latest/meta-data/public-ipv4

#Private IP
#curl http://169.254.169.254/latest/meta-data/local-ipv4

#Public Hostname
PHN=`curl http://169.254.169.254/latest/meta-data/public-hostname`

echo "Public DNS is $PHN"
cd ~/hadoop/etc/hadoop

sed -i -e "s/public_dns_namenode/${PHN}/g" masters
sed -i -e "s/public_dns_namenode/${PHN}/g" core-site.xml
sed -i -e "s/public_dns_namenode/${PHN}/g" yarn-site.xml
sed -i -e "s/public_dns_namenode/${PHN}/g" mapred-site.xml

echo "ec2-52-38-48-46.us-west-2.compute.amazonaws.com" >> slaves
echo "ec2-52-37-211-160.us-west-2.compute.amazonaws.com" >> slaves