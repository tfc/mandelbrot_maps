
#ifndef __JPEGMAP_H__
#define __JPEGMAP_H__

#include <string>
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

		RgbColor blurPixel(const unsigned x, const unsigned y) const;

	public:
		int read_jpeg_file(const std::string &filename);
		int write_jpeg_file(const std::string &filename, const unsigned quality = 75) const;

		void blur();
		
		RgbColor getPixel(const unsigned x, const unsigned y) const;
		void setPixel(const unsigned x, const unsigned y, const RgbColor &color);
	
		unsigned width()  const { return _width; }
		unsigned height() const { return _height; }

		void reinit(const unsigned width, const unsigned height)
		{
			if (_raw != NULL) delete _raw;
			_raw = NULL;

			_width = width;
			_height = height;
			_bpp = 3;
			_color_space = JCS_RGB;

			if (width && height)
				_raw = new unsigned char[width * height * _bpp];
		}

		JpegMap()
			: _raw(NULL)
		{ reinit(0, 0); }

		JpegMap(const unsigned width, const unsigned height)
			: _raw(NULL)
		{ reinit(width, height); }

		~JpegMap() { if (_raw) delete _raw; }
};

#endif

