#include "jpegmap.h"

int JpegMap::read_jpeg_file(const char *filename)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];

	FILE *infile = fopen(filename, "rb");
	unsigned long location = 0;

	if (!infile) {
		printf("Error opening jpeg file %s\n!", filename);
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);

	_width = cinfo.image_width;
	_height = cinfo.image_height;
	_bpp = cinfo.num_components;
	//_color_space = JCS_RGB; //cinfo.jpeg_color_space;

	jpeg_start_decompress(&cinfo);
	_raw = new unsigned char[_width * _height * _bpp];

	row_pointer[0] = new unsigned char[_width * _bpp];
	while (cinfo.output_scanline < cinfo.image_height) {
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		for(int i=0; i < _width * _bpp; i++) {
			_raw[location] = row_pointer[0][i];
			++location;
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	delete row_pointer[0];
	fclose(infile);

	return 0;
}

int JpegMap::write_jpeg_file(const char *filename, const unsigned quality)
{
	if (!_raw || !_width || !_height)
		return -1;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];
	FILE *outfile = fopen(filename, "wb");

	if (!outfile) {
		printf("Error opening output jpeg file %s\n!", filename);
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = _width;
	cinfo.image_height = _height;
	cinfo.input_components = _bpp;
	cinfo.in_color_space = _color_space;

	jpeg_set_defaults(&cinfo);
	cinfo.num_components = 3;
	//cinfo.data_precision = 4;
	cinfo.dct_method = JDCT_FLOAT;
	jpeg_set_quality(&cinfo, quality, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	while(cinfo.next_scanline < _height) {
		row_pointer[0] = &_raw[cinfo.next_scanline * _width * _bpp];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);

	return 0;
}

void JpegMap::setPixel(const unsigned x, const unsigned y, const RgbColor color)
{
	const unsigned offset = ( x + y * _width ) * _bpp;
	//if (x >= _width || y >= _height) return;

	_raw[offset]     = color.b;
	_raw[offset + 1] = color.g;
	_raw[offset + 2] = color.r;
}
