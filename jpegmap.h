
#ifndef __JPEGMAP_H__
#define __JPEGMAP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>

#include "colors.h"

class JpegMap
{
	private:
		unsigned char *_raw;

		unsigned int _width;
		unsigned int _height;
		unsigned int _bpp; 
		J_COLOR_SPACE _color_space;

	public:
		int read_jpeg_file(const char *filename);
		int write_jpeg_file(const char *filename, const unsigned quality = 75);
		
		void setPixel(const unsigned x, const unsigned y, const RgbColor color);
	
		unsigned width()  const { return _width; }
		unsigned height() const { return _height; }

		JpegMap()
			: _raw(NULL), _width(0), _height(0), _bpp(0), _color_space(JCS_RGB)
		{}

		JpegMap(const unsigned width, const unsigned height)
			: _raw(new unsigned char[width * height * 3]), 
			_width(width), _height(height), _bpp(3), _color_space(JCS_RGB)
		{}

		~JpegMap() { if (_raw) delete _raw; }
};

#endif

