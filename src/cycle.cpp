#include <cmath>
#include <cstdio>
#include <complex>
#include <cstring>

#include <assert.h>

#include "../include/bmp.h"

std::complex<double> convert(int x, int y, int unit) {
	return std::complex<double>((double)x / unit, (double)y / unit);
}

COLOR_RGBA colorset[7] = {WHITE, 
						  COLOR_RGBA(255, 246, 143, 0), 
						  COLOR_RGBA(127, 255, 0, 0), 
						  COLOR_RGBA(32, 178, 170, 0), 
						  COLOR_RGBA(255, 130, 71, 0), 
						  COLOR_RGBA(148, 0, 211, 0)};

COLOR_RGBA colors(int num) {
	return colorset[num];
}

int main(int argc, char* argv[]) {
	// iternum is the maxinum iteration time
	int iternum = 100;
	// unit = pixels in one unit (0, 0)->(0, 1)
	int unit = 500;
	if (argc == 2) {
		sscanf(argv[1], "%d", &iternum);
	} else if(argc == 3) {
		sscanf(argv[1], "%d", &iternum); 
		sscanf(argv[2], "%d", &unit);
	}
	// size of the picture
	// int width = 1920, height = 1080;
	int height = unit * 3, width = unit * 3;
	std::complex<double> origin = std::complex<double>(-2, -1.5);
	// pic is the picture plane
	unsigned char* pic = new unsigned char [width * height];
	memset(pic, 0, sizeof(pic));

	// generate the picture
	// cur is for "current position", because of the choice of iteraters, next position is just cur++.
	unsigned char* cur = pic;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			std::complex<double> c = convert(i, j, unit) + origin;

			int flag = true;
			std::complex<double> z = 0;
			// #pragma omp parallel for shared(flag)
			for (int itertime = 0; itertime < iternum; itertime += 4) {
				// if (!flag) continue;
				z = z * z + c; flag = (std::norm(z) > 4) ? false : flag;
				z = z * z + c; flag = (std::norm(z) > 4) ? false : flag;
				z = z * z + c; flag = (std::norm(z) > 4) ? false : flag;
				z = z * z + c; flag = (std::norm(z) > 4) ? false : flag;
				if (flag == false) break;
			}
			*cur = 0;
			if (flag) {
				std::complex<double> last = z;
				while (*cur < 5) {
					z = z * z + c;
					(*cur)++;
					if (std::norm(z - last) < 1e-8) break;
				}
			}
			cur++;
		}
	}

	// write file
	char *filename = new char [20 + 2 + 20];
	sprintf(filename, "../img/cycle-%d-%d.bmp", iternum, unit);
	FILE *fout = fopen(filename, "w");
	WriteBMP(fout, pic, width, height, 24, colors);
	delete[] filename;

	fclose(fout);
	delete[] pic;
}