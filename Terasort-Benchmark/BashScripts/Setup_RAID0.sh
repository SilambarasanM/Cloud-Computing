#!/bin/bash
echo"Setting up RAID0..."
echo "Unmounting ephemeral0"
sudo umount -l /media/ephemeral0

echo "Formatting storage..."
sudo mkfs -F -t ext4 /dev/xvdb
sudo mkfs -F -t ext4 /dev/xvdc
#EBS volume
#sudo mkfs -F -t ext4 /dev/xvdc

echo "Creating RAID0..."
sudo mdadm --create --verbose /dev/md0 --level=0 --name=MY_RAID --raid-devices=2 /dev/xvdb /dev/xvdc
#EBS Volume
#sudo mdadm --create --verbose /dev/md0 --level=0 --name=MY_RAID --raid-devices=3 /dev/xvdb /dev/xvdc /dev/xvdd
sudo mkfs.ext4 -L MY_RAID /dev/md0

echo "Mounting RAID0..."
sudo mkdir -p /mnt/raid0
sudo mount LABEL=MY_RAID /mnt/raid0

lsblk

#Changing Permissions
sudo chown -R ec2-user /mnt/raid0
sudo chgrp -R ec2-user /mnt/raid0
