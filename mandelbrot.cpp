#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>

#include "mandelbrot.h"

typedef std::complex<double> cmplx;

static inline double squ_abs(const cmplx vec)
{
	return vec.real() * vec.real() + vec.imag() * vec.imag();
}

static inline cmplx complex_coord(const unsigned width, const unsigned height, 
		const double x, const double y, 
		const double zoom = 4)
{
	double real = static_cast<double>(x - (height / 2)) / height * zoom;
	double imag = static_cast<double>(y - (height / 2)) / height * zoom;

	return cmplx(real, imag);
}

static inline int mandelbrot_iterations(const cmplx pos, const unsigned max_iterations,
		cmplx &outvector)
{
	cmplx z = 0;
	unsigned iteration = 0;

	if (squ_abs(pos + cmplx(0.4, 0)) > 3.2) return max_iterations;
	double q = squ_abs(pos - cmplx(0.25, 0));
	if (q * ( q + (pos.real() - 0.25) ) < pos.imag() * pos.imag() / 4) return max_iterations;
	if (squ_abs(pos + cmplx(1, 0)) < 1.0 / 16.0) return max_iterations;

	while (squ_abs(z) < 4 && iteration < max_iterations) {
		z = z*z + pos;
		++iteration;
	}

	outvector = z;

	return iteration; // - log(   log(squ_abs(z))/2.0 / log(max_iterations)   ) / log(2);
}

static inline RgbColor rgb_mandelbrot(const int its, const int max_iterations, const cmplx lastvector)
{
	if (its == 0 || its == max_iterations) return RgbColor(0, 0, 0);

	const double smoothed = its - log(   log(squ_abs(lastvector))/2.0 / log(max_iterations)   ) / log(2);
	const unsigned brightness = std::min(std::pow(std::max(smoothed-6, 0.0), 2), 255.0);
	HsvColor hsv(static_cast<unsigned>(its) % 256, 255, brightness);
	return RgbColor(hsv);
}

static inline int paint_pixel(const unsigned total_width, const unsigned total_height,
		const unsigned img_width, const unsigned img_height,
		const unsigned start_x, const unsigned x,
		const unsigned start_y, const unsigned y,
		const unsigned iterations,
		JpegMap &img, JpegMap &img_mirror)
{
	const cmplx pos = complex_coord(total_width, total_height, start_x + x, start_y + y);
	cmplx lastvector;
	const int val = mandelbrot_iterations(pos, iterations, lastvector);

	const RgbColor col = rgb_mandelbrot(val, iterations, lastvector);
	img.setPixel(x, y, col);
	img_mirror.setPixel(x, img_height - 1 - y, col);

	return val;
}

static inline double paint_pixels(const unsigned total_width, const unsigned total_height,
		const unsigned img_width, const unsigned img_height,
		const unsigned start_x, const unsigned start_y,
		const unsigned lx, const unsigned rx,
		const unsigned uy, const unsigned ly,
		const unsigned iterations,
		JpegMap &img, JpegMap &img_mirror)
{
	if (std::min(rx-lx, ly-uy) < 10) {
		double maxval = 0;
		for (int y=uy; y < ly; ++y) {
			for (int x=lx; x < rx; ++x) {
				const double val = paint_pixel(total_width, total_height,
				img_width, img_height,
				start_x, x,
				start_y, y,
				iterations, img, img_mirror);

				maxval = std::max(val, maxval);
			}
		}
		return maxval;
	}
	if (rx <= lx || ly <= uy) return 0;

	double maxdiff = 0;
	cmplx z;
	const cmplx pos = complex_coord(total_width, total_height, start_x, start_y);
	const int firstval = mandelbrot_iterations(pos, iterations, z);

	for (int y = uy+1; y < ly - 1; ++y) {
		// Left column
		const int val = paint_pixel(total_width, total_height,
				img_width, img_height,
				start_x, lx,
				start_y, y,
				iterations, img, img_mirror);
		// Right column
		const int val2 = paint_pixel(total_width, total_height,
				img_width, img_height,
				start_x, rx-1,
				start_y, y,
				iterations, img, img_mirror);
		maxdiff = std::max(maxdiff, std::fabs(val - firstval));
		maxdiff = std::max(maxdiff, std::fabs(val2 - firstval));
	}

	for (int x = lx; x < rx; ++x) {
		// Upper row
		const int val = paint_pixel(total_width, total_height,
				img_width, img_height,
				start_x, x,
				start_y, uy,
				iterations, img, img_mirror);
		// Lower row
		const int val2 = paint_pixel(total_width, total_height,
				img_width, img_height,
				start_x, x,
				start_y, ly-1,
				iterations, img, img_mirror);
		maxdiff = std::max(maxdiff, std::fabs(val - firstval));
		maxdiff = std::max(maxdiff, std::fabs(val2 - firstval));
	}

	if (maxdiff == 0) {
		const RgbColor col = rgb_mandelbrot(firstval, iterations, z);
		for (int y=uy; y < ly; ++y) {
			for (int x=lx; x < rx; ++x) {
				img.setPixel(x, y, col);
				img_mirror.setPixel(x, img_height - 1 - y, col);
			}
		}

		return firstval;
	}

	const unsigned mx = (rx + lx) / 2;
	const unsigned my = (ly + uy) / 2;

	int maxval = 0;
	int ret;

	// Upper left
	ret = paint_pixels(total_width, total_height,
			img_width, img_height,
			start_x, start_y,
			lx+1, mx, uy+1, my,
			iterations,
			img, img_mirror);
	maxval = std::max(ret, maxval);
	// Upper right
	ret = paint_pixels(total_width, total_height,
			img_width, img_height,
			start_x, start_y,
			mx, rx-1, uy+1, my,
			iterations,
			img, img_mirror);
	maxval = std::max(ret, maxval);
	// Lower left
	ret = paint_pixels(total_width, total_height,
			img_width, img_height,
			start_x, start_y,
			lx, mx, my, ly-1,
			iterations,
			img, img_mirror);
	maxval = std::max(ret, maxval);
	// Lower right
	ret = paint_pixels(total_width, total_height,
			img_width, img_height,
			start_x, start_y,
			mx, rx-1, my, ly-1,
			iterations,
			img, img_mirror);
	maxval = std::max(ret, maxval);

	return maxval;
}

void paint_part(const unsigned total_width, const unsigned total_height,
		const unsigned total_chunks_x, const unsigned total_chunks_y,
		const unsigned chunk_x, const unsigned chunk_y,
		const unsigned iterations,
		const std::string filename_prefix,
		const bool mirror_optimization)
{
	const unsigned img_width = total_width / total_chunks_x;
	const unsigned img_height = total_height / total_chunks_y;

	const unsigned start_x = img_width * chunk_x;
	const unsigned start_y = img_height * chunk_y;

	const unsigned chunk_y_mirror = total_chunks_y - 1 - chunk_y;

	JpegMap img(img_width, img_height);
	JpegMap img_mirror;
	if (mirror_optimization) 
		img_mirror.reinit(img_width, img_height);

	int maxval = paint_pixels(total_width, total_height,
			img_width, img_height,
			start_x, start_y,
			0, img_width,
			0, img_height,
			iterations,
			img, img_mirror);

	// If valcnt is zero, then every pixel is black.
	// Don't waste disk space for that.
	if (maxval < 5) return;

	img.blur();
	img_mirror.blur();

	{
	std::stringstream file;
	file << filename_prefix << "mb_" << chunk_x << "_" << chunk_y << ".jpg";
	img.write_jpeg_file(file.str().c_str(), 95);
	}

	if (mirror_optimization) {
		std::stringstream file;
		file << filename_prefix << "mb_" << chunk_x << "_" << chunk_y_mirror << ".jpg";
		img_mirror.write_jpeg_file(file.str().c_str(), 95);
	}
}

