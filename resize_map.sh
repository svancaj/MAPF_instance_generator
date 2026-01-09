#!/bin/bash


for map in maps/*.map
do
	size=$(echo $map | cut -d_ -f2 | cut -d. -f1)
	lines=$(($size + 4))
	
	cat $map | cut -c 1-$size | head -n $lines > tmp && cat tmp > $map

	rm tmp

done
