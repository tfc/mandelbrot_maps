
#ifndef __MANDELBROT_H__
#define __MANDELBROT_H__

#include "jpegmap.h"
#include "colors.h"

void paint_part(const unsigned total_width, const unsigned total_height,
		const unsigned total_chunks_x, const unsigned total_chunks_y,
		const unsigned chunk_x, const unsigned chunk_y,
		const unsigned iterations,
		const std::string filename_prefix,
		const bool mirror_optimization = false);

#endif
