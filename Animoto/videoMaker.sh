#!/bin/sh

#Creating directories for each task
mkdir images$1
cd images$1

#Downloading Images
wget -i ../images$1.txt -o download.log

#Renaming image files using links
x=1
for i in *jpg
do
	counter=$(printf %d $x)
	ln -s "$i" img"$counter".jpg
	x=$(($x+1))
done

#Creating Video
ffmpeg -framerate 1 -i img%d.jpg video$1.mkv

#Moving Video to python script location
mv video$1.mkv ../.

#Cleanup
rm *.jpg*
cd ..
rm -r images*