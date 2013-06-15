#!/bin/bash

for zoom in $(seq $1 $2); do
	size=$(( 2 ** ( 10 + $zoom ) ))

	./mb $size 256 $zoom 10000
done
