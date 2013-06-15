#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <complex>
#include <omp.h>
#include <time.h>

#include "jpegmap.h"
#include "colors.h"


using std::cout;
using std::endl;

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

static inline int mandelbrot_iterations(const cmplx pos, const unsigned max_iterations)
{
	cmplx z = 0;
	unsigned iteration = 0;

	if (squ_abs(pos - cmplx(-0.4, 0)) > 3.2) return -1;
	if (squ_abs(pos - cmplx(-1, 0)) < 0.05) return -1;
	if (pos.real() < 0 && squ_abs(pos - cmplx(-0.1, 0)) < 0.4) return -1;
	if (pos.real() >= 0 && squ_abs(pos + cmplx(-0.16, 0)) < pow(arg(pos + cmplx(-0.16, 0)) / 2.8, 2)) return -1;

	while (squ_abs(z) < 4 && iteration < max_iterations) {
		z = z*z + pos;
		++iteration;
	}

	return iteration - log(log(squ_abs(z) / log(iteration)));
}

static inline RgbColor rgb_mandelbrot(int its)
{
	if (its < 0) {
		return RgbColor(0x00, 0x00, 0x00);
	}

	HsvColor hsv(its % 256, 255, 255 - 255 / (its +1));
	return RgbColor(hsv);
}

void paint_part(const unsigned total_width, const unsigned total_height,
		const unsigned total_chunks_x, const unsigned total_chunks_y,
		const unsigned chunk_x, const unsigned chunk_y,
		const unsigned iterations,
		const std::string filename_prefix,
		const bool mirror_optimization = false)
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

#pragma omp parallel for schedule(dynamic, 8)
	for (int y = 0; y < img_height; ++y) {
		for (int x = 0; x < img_width; ++x) {
			const cmplx pos = complex_coord(total_width, total_height, 
					x + start_x, y + start_y);
			const int val = mandelbrot_iterations(pos, iterations) * 10;
			const RgbColor col = rgb_mandelbrot(val);
			img.setPixel(x, y, col);
			img_mirror.setPixel(x, img_height - 1 - y, col);
		}
	}

	img.blur();
	img_mirror.blur();

	{
	std::stringstream file;
	file << filename_prefix << "mb_" << chunk_x << "_" << chunk_y << ".jpg";
	img.write_jpeg_file(file.str().c_str(), 75);
	}

	if (mirror_optimization) {
		std::stringstream file;
		file << filename_prefix << "mb_" << chunk_x << "_" << chunk_y_mirror << ".jpg";
		img_mirror.write_jpeg_file(file.str().c_str(), 75);
	}
}

int main(int argc, char **argv)
{
	if (argc != 5) {
		cout << "Usage: " << argv[0] << " <xy dim>"
			<< "<tile size> <zoom> <iterations>" << endl;
		return 1;
	}

	unsigned size = atoi(argv[1]);
	const unsigned chunk_size = atoi(argv[2]);
	size = (size + chunk_size - 1) / chunk_size * chunk_size;
	const unsigned chunks = size / chunk_size;
	const unsigned zoom = atoi(argv[3]);
	const unsigned iterations = atoi(argv[4]);

	const bool mirror_optimization = chunks % 2 == 0;
	const unsigned y_chunks = (mirror_optimization) ? chunks / 2 : chunks;

	std::stringstream fs_prefix;
	fs_prefix << "pics/" << zoom << "/";

	double start = omp_get_wtime();

	for (unsigned i=0; i < y_chunks; ++i) {
		for (unsigned j=0; j < chunks; ++j) {
			paint_part(size, size, chunks, chunks, j, i, iterations, 
					fs_prefix.str(), mirror_optimization);
		}
	}

	double end = omp_get_wtime();

	cout << "time: " << (end - start) << " s" << endl;

	return 0;
}

