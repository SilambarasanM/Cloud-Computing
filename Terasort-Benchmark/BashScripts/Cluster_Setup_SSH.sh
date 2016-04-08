#!/bin/bash

#Setting up Password-less SSH
echo "Setting up Password-less SSH"
ssh-keygen -f ~/.ssh/id_rsa -t rsa -N ''
cat .ssh/id_rsa.pub >> .ssh/authorized_keys
chmod 600 .ssh/authorized_keys
echo "StrictHostKeyChecking=no" >> ~/.ssh/config

#Installing PSSH
echo "Installing Python PIP..."
sudo yum install python pip
echo"Installing PSSH..."
sudo yum install pssh

#Setting up hostnames for nodes in cluster for ease of access
echo "Host namenode" >>  ~/.ssh/config
echo "	HostName ec2-54-175-40-159.compute-1.amazonaws.com" >>  ~/.ssh/config
echo "	User ec2-user" >>  ~/.ssh/config
echo "	IdentityFile ~/.ssh/key.pem" >>  ~/.ssh/config

echo "Host datanode1" >>  ~/.ssh/config
echo "	HostName ec2-52-201-225-81.compute-1.amazonaws.com" >>  ~/.ssh/config
echo "	User ec2-user" >>  ~/.ssh/config
echo "	IdentityFile ~/.ssh/key.pem" >>  ~/.ssh/config

chmod 600 ~/.ssh/config


#Adding hostnames to etc/hosts
echo "ec2-54-175-154-22.compute-1.amazonaws.com namenode" >> /etc/hosts
echo "ec2-52-201-225-81.compute-1.amazonaws.com datanode1" >> etc/hosts

#Preparing hostnames file for PSSH
echo "datanode1" >> hostnames
echo "datanode2" >> hostnames

#Copying Master key to all datanodes
#cat .ssh/id_rsa.pub | ssh -i key.pem datanode1 'cat >> ~/.ssh/authorized_keys'
#cat .ssh/id_rsa.pub | ssh -i key.pem datanode2 'cat >> ~/.ssh/authorized_keys'
cat ~/.ssh/id_rsa.pub | pssh -h hostnames -l ec2-user -P 'cat >> ~/.ssh/authorized_keys'

#Transfer installation file to datanodes
pscp -h hostnames -l ec2-user Setup.sh ~

#Execute scripts on all machines with no time-out
pssh -h hostnames -t -1 './Setup.sh'