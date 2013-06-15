mandelbrot_maps
===============

A C++ program which renders a visualization of the 
famous Mandelbrot set and produces tiled image output 
for the Google Maps API.

Usage:

1.) run "make" to compile it.
2.) run "./make_tiles 0 7" to generate zoom steps 0 to 7
3.) open "map.html" to look at the result. 

The Google Maps code included in map.html is configured to use zoom
steps from 0 to 7. More or less is possible, just edit the HTML code.
