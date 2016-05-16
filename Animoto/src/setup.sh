#!/bin/sh

#Preparing Crendentials for Python Script execution
mkdir .aws
mv credentials .aws/

#Installing FFMPEG
sudo yum update
sudo yum install gcc-c++ make yasm pkgconfig libXext-devel libXfixes-devel x264-devel zlib-devel
wget http://ffmpeg.org/releases/ffmpeg-3.0.2.tar.bz2
tar xjvf ffmpeg-3.0.2.tar.bz2
cd ffmpeg-3.0.2
sudo ./configure
sudo make
sudo make install

#Running workers
#pssh -h workers -l ec2-user -P -t 100000000000 'python RemoteWorker.py -t 2'
#pssh -h workers -l ec2-user -P -t 100000000000 'python RemoteWorker.py -t 2 -a'

#Setting up SSH for Workers
#ssh-keygen -f ~/.ssh/id_rsa -t rsa -N ''
#cat .ssh/id_rsa.pub >> .ssh/authorized_keys
#chmod 600 .ssh/authorized_keys
#echo "StrictHostKeyChecking=no" >> ~/.ssh/config

#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-165-202-14.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-164-99-22.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-52-87-192-2.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-164-79-46.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-164-124-185.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-84-38-187.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-164-105-235.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-164-48-121.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-52-87-190-231.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-52-91-114-130.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-52-90-66-173.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-152-158-63.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-52-91-106-109.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-152-227-103.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-52-91-168-255.compute-1.amazonaws.com:~/.ssh/authorized_keys
#scp -i Cloud_PA3.pem authorized_keys ec2-user@ec2-54-89-144-104.compute-1.amazonaws.com:~/.ssh/authorized_keys