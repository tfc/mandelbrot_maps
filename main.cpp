#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <math.h>
#include <complex>
#include <omp.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#include "jpegmap.h"
#include "colors.h"
#include "mandelbrot.h"

using std::cout;
using std::endl;

bool mkpath(std::string path)
{
    bool success = false;
    int ret = mkdir(path.c_str(), 0775);
    if (ret == -1) {
        switch(errno) {
            case ENOENT:
                if (mkpath(path.substr(0, path.find_last_of('/'))))
                    success = 0 == mkdir(path.c_str(), 0775);
                else
                    success = false;
                break;
            case EEXIST:
                success = true;
                break;
            default:
                success = false;
                break;
        }
    }
    else
        success = true;

    return success;
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
	fs_prefix << "./pics/" << zoom << "/";

	if (!mkpath(fs_prefix.str().c_str())) {
		cout << "Unable to create directory \"./pics/" << zoom << "\"" << endl;
		return 1;
	}

	double start = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, 1)
	for (int i=0; i < y_chunks; ++i) {
		for (int j=0; j < chunks; ++j) {
			paint_part(size, size, chunks, chunks, j, i, iterations, 
					fs_prefix.str(), mirror_optimization);
		}
	}

	double end = omp_get_wtime();

	cout << "size " << std::setw(10) << size 
		<< " csize " << std::setw(10) << chunk_size
		<< " its " << std::setw(10) << iterations
		<< " time " << std::fixed << std::setw(13) << std::setprecision(3) 
		<< (end - start) << " s" << endl;

	return 0;
}

