#!/bin/sh

for zoom in $(seq 7 8); do
	size=$(( 2 ** ( 10 + $zoom ) ))
	echo "zoom $zoom size $size"

	rm -rf pics/$zoom
	mkdir pics/$zoom

	./main $size 256 $zoom 5000
done
