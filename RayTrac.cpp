#include <algorithm>
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

#include "geometry.h"

using namespace std;

struct Light
{
	Light(const Vec3f &p, const float &i) : position(p), intensity(i) {}
	Vec3f position;
	float intensity;
};

struct Material
{
	Material(const Vec3f &color) : diffuse_color(color) {}
	Material() : diffuse_color() {}
	Vec3f diffuse_color;
};

struct Orbs
{
	Vec3f center;
	float radius;
	Material material;

	Orbs(const Vec3f &c, const float &r, const Material &m) : center(c), radius(r), material(m) {}

	bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const
	{
		Vec3f L = center - orig;
		float tca = L * dir;
		float d2 = L * L - tca * tca;

		if (d2 > radius*radius) return false;

		float thc = sqrtf(radius*radius - d2);
		t0 = tca - thc;

		float t1 = tca + thc;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;

		return true;
	}
};

bool scene_intersect(const Vec3f &orig, const Vec3f &dir, const vector<Orbs> &Orbs, Vec3f &hit, Vec3f &N, Material &material)
{
	float Orbs_dist = std::numeric_limits<float>::max();
	for (size_t i = 0; i < Orbs.size(); i++)
	{
		float dist_i;
		if (Orbs[i].ray_intersect(orig, dir, dist_i) && dist_i < Orbs_dist)
		{
			Orbs_dist = dist_i;
			hit = orig + dir * dist_i;
			N = (hit - Orbs[i].center).normalize();
			material = Orbs[i].material;
		}
	}
	return Orbs_dist < 1000;
}

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Orbs> &Orbs, const std::vector<Light> &lights)
{
	Vec3f point, N;
	Material material;

	if (!scene_intersect(orig, dir, Orbs, point, N, material))
	{
		return Vec3f(0.0, 0.0, 0.0); // background color
	}

	float diffuse_light_intensity = 0;
	for (size_t i = 0; i < lights.size(); i++)
	{
		Vec3f light_dir = (lights[i].position - point).normalize();
		diffuse_light_intensity += lights[i].intensity * max(0.f, light_dir*N);
	}
	return material.diffuse_color * diffuse_light_intensity;
}

void render(const std::vector<Orbs> &Orbs, const std::vector<Light> &lights)
{
	const int width = 1024;
	const int height = 768;
	const int fov = 3.1415926535897932384 / 2.;
	vector<Vec3f> framebuffer(width*height);

	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			float x = (2 * (i + 0.5) / (float)width - 1)*tan(fov / 2.)*width / (float)height;
			float y = -(2 * (j + 0.5) / (float)height - 1)*tan(fov / 2.);
			Vec3f dir = Vec3f(x, y, -1).normalize();
			framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir, Orbs, lights);
		}
	}

	ofstream ofs("./illustration.ppm", ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height*width; ++i)
	{
		for (size_t j = 0; j < 3; j++)
		{
			ofs << (char)(max(0, min(255, static_cast<int>(255 * framebuffer[i][j]))));
		}
	}
	ofs.close();
}

int main()
{
	Material sf1(Vec3f(0.0, 0.128, 1.28));
	Material sf2(Vec3f(1.32, 1.32, 1.30));

	vector<Orbs> Orb;
	Orb.push_back(Orbs(Vec3f(-3.26, .0, -15.98), 4, sf1));
	Orb.push_back(Orbs(Vec3f(6.23, 0.456, -12.89), 1.0, sf2));

	vector<Light>  lights;
	lights.push_back(Light(Vec3f(-45.0, 25.0, 15.0), 1.5));

	render(Orb, lights);

	return 0;
}
