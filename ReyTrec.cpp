// ReyTrec.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define _USE_MATH_DEFINES
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <limits>
#include <cmath>
#include <fstream>
#include <vector>
#include <algorithm>
#include "geometry.h"
#include "stb_image_write.h"
using namespace std;

	struct Sphere {
		Vec3f center;
		float radius;

		Sphere(const Vec3f &c, const float &r) : center(c), radius(r) {}

		bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
			Vec3f L = center - orig;
			float tca = L*dir;
			float d2 = L*L - tca*tca;
			if (d2 > radius*radius) return false;
			float thc = sqrtf(radius*radius - d2);
			t0 = tca - thc;
			float t1 = tca + thc;
			if (t0 < 0) t0 = t1;
			if (t0 < 0) return false;
			return true;
		}
	};

	Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const Sphere &sphere) {
		float sphere_dist = std::numeric_limits<float>::max();
		if (!sphere.ray_intersect(orig, dir, sphere_dist)) {
			return Vec3f(0.2, 0.7, 0.8); // background color
		}

		return Vec3f(0.4, 0.4, 0.3);
	}

	void render(const Sphere &sphere) {
		const int width = 1024;
		const int height = 768;
		const int fov = M_PI / 2.;
		std::vector<Vec3f> framebuffer(width*height);

#pragma omp parallel for
		for (size_t j = 0; j<height; j++) {
			for (size_t i = 0; i<width; i++) {
				float x = (2 * (i + 0.5) / (float)width - 1)*tan(fov / 2.)*width / (float)height;
				float y = -(2 * (j + 0.5) / (float)height - 1)*tan(fov / 2.);
				Vec3f dir = Vec3f(x, y, -1).normalize();
				framebuffer[i + j*width] = cast_ray(Vec3f(0, 0, 0), dir, sphere);
			}
		}

		ofstream ofs; // save the framebuffer to file
		ofs.open("./out.jpg");
		ofs << "P6\n" << width << " " << height << "\n255\n";
		for (size_t i = 0; i < height*width; ++i) {
			for (size_t j = 0; j<3; j++) {
				ofs << (char)(255 * max(0.f, min(1.f, framebuffer[i][j])));
				//framebuffer[i][j] = (255 * max(0.f, min(1.f, framebuffer[i][j])));
				//stbi_write_jpg("image.jpg", nx, ny, 3, Sphere, 100);

			}
		}
		ofs.close();
		
	}

	struct RGB {
		unsigned char R;
		unsigned char G;
		unsigned char B;
	};

	int main() {
		Sphere sphere(Vec3f(-3, 0, -16), 2);
		render(sphere);

		/*const int nx = 1980;
		const int ny = 1070;

		struct RGB data[nx][ny];

		for (int j = ny - 1; j >= 0; j--) {
			for (int i = 0; i < nx; i++) {

				float r = float(i) / float(nx);
				float g = float(ny - 1 - j) / float(ny);
				float b = 0.2;

				int ir = int(255.99 * r);
				int ig = int(255.99 * g);
				int ib = int(255.99 * b);

				data[i][j].R = ir;
				data[i][j].G = ig;
				data[i][j].B = ib;
			}
		}
		stbi_write_jpg("image.jpg", nx, ny, 3, data, 100);*/

		return 0;
	}