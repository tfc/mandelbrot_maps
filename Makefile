

default: main

main: main.cpp
	g++ -o mb main.cpp jpegmap.cpp -ljpeg -lm -ffast-math -fcx-limited-range -fopenmp -O3

clean:
	rm -f mb
