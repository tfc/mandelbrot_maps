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

static inline double mandelbrot_iterations(const cmplx pos, const unsigned max_iterations)
{
	cmplx z = 0;
	unsigned iteration = 0;

	if (squ_abs(pos + cmplx(0.4, 0)) > 3.2) return 0;
	double q = squ_abs(pos - cmplx(0.25, 0));
	if (q * ( q + (pos.real() - 0.25) ) < pos.imag() * pos.imag() / 4) return -0;
	if (squ_abs(pos + cmplx(1, 0)) < 1.0 / 16.0) return -0;

	while (squ_abs(z) < 4 && iteration < max_iterations) {
		z = z*z + pos;
		++iteration;
	}

	return iteration - log(   log(squ_abs(z))/2.0 / log(max_iterations)   ) / log(2);
}

static inline RgbColor rgb_mandelbrot(double its)
{
	const unsigned brightness = std::min(std::pow(std::max(its-6, 0.0), 2), 255.0);
	HsvColor hsv(static_cast<unsigned>(its) % 256, 255, brightness);
	return RgbColor(hsv);
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

	int maxval = 0;

	for (int y = 0; y < img_height; ++y) {
		for (int x = 0; x < img_width; ++x) {
			const cmplx pos = complex_coord(total_width, total_height, 
					x + start_x, y + start_y);
			const double val = mandelbrot_iterations(pos, iterations);

			const RgbColor col = rgb_mandelbrot(val);
			img.setPixel(x, y, col);
			img_mirror.setPixel(x, img_height - 1 - y, col);

			maxval = std::max(maxval, static_cast<int>(val));
		}
	}

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

