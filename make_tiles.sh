#!/bin/sh

for zoom in $(seq $1 $2); do
	size=$(( 2 ** ( 10 + $zoom ) ))
	echo "zoom $zoom size $size"

	rm -rf pics/$zoom
	mkdir pics/$zoom

	./mb $size 256 $zoom 5000
done
