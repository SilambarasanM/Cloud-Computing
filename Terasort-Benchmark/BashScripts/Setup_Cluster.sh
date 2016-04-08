#!/bin/bash

pssh -h hostnames -l ec2-user -P 'lsblk'

pssh -h hostnames -l ec2-user -P 'sudo umount -l /media/ephemeral0'
pssh -h hostnames -l ec2-user -P 'sudo mkfs -F -t ext4 /dev/xvdb'
pssh -h hostnames -l ec2-user -P 'sudo mkfs -F -t ext4 /dev/xvdc'
pssh -h hostnames -l ec2-user -P 'sudo mkfs -F -t ext4 /dev/xvdd'
pssh -h hostnames -l ec2-user -P 'sudo mkfs.ext4 -L MY_RAID /dev/md0'

echo "yes" | ssh  datanode1 'sudo mdadm --create --verbose /dev/md0 --level=0 --name=MY_RAID --raid-devices=3 /dev/xvdb /dev/xvdc /dev/xvdd'
echo "yes" | ssh  datanode2 'sudo mdadm --create --verbose /dev/md0 --level=0 --name=MY_RAID --raid-devices=3 /dev/xvdb /dev/xvdc /dev/xvdd'

pssh -h hostnames -l ec2-user -P 'sudo mkdir -p /mnt/raid0'
pssh -h hostnames -l ec2-user -P 'sudo mount LABEL=MY_RAID /mnt/raid0'
pssh -h hostnames -l ec2-user -P 'sudo mkdir -p /mnt/raid0'
pssh -h hostnames -l ec2-user -P 'sudo chown -R ec2-user /mnt/raid0'
pssh -h hostnames -l ec2-user -P 'sudo chgrp -R ec2-user /mnt/raid0'
pssh -h hostnames -l ec2-user -P 'sudo mkdir -p /mnt/raid0/hdfs/datanode'
pscp -h hostnames -l ec2-user datanode_update.sh ~


cd ~/hadoop/etc/hadoop
sed -i "s/namenode/datanode/g" hdfs-site.xml
sed -i -e "s/public_dns_namenode/ec2-52-36-166-118.us-west-2.compute.amazonaws.com/g" masters
sed -i -e "s/public_dns_namenode/ec2-52-36-166-118.us-west-2.compute.amazonaws.com/g" mapred-site.xml
sed -i -e "s/public_dns_namenode/ec2-52-36-166-118.us-west-2.compute.amazonaws.com/g" yarn-site.xml
sed -i -e 's/ec2-54-173-44-127.compute-1.amazonaws.com/ec2-52-36-166-118.us-west-2.compute.amazonaws.com/g' core-site.xml
