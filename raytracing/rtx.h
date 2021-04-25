//
// Created by Yaroslav Okatev on 14.03.2021.
//

#ifndef RTX_RTX_H
#define RTX_RTX_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <fstream>
#include "geometry.h"

const float EPSILON = 0.0000001;
const float FLARE_COEFF = 0.5, FLARE_POW = 100;
const float LIGHT_BASE = 100, LIGHT_COEFF = 0.5;
const unsigned int DEPTH = 5;

typedef struct Intersection {
	bool isIntersected;
	char side;
	Vec3f intersectionPoint;

	Intersection(): isIntersected(false) {};
} Intersection;

typedef struct Triangle {
	Vec3f v0;
	Vec3f v1;
	Vec3f v2;
	Vec3f normal;

	Triangle(const Vec3f &p0, const Vec3f &p1, const Vec3f &p2) {
		v0 = p0;
		v1 = p1;
		v2 = p2;
		normal = cross(v1 - v0, v2 - v0).normalize();
	};
} Triangle;


void readTrianglesFromFile(std::string fileName, std::vector<Triangle> &trs, int amount);
void saveImage(char* filename, int w, int h, unsigned char* data);
Intersection rayTriangleIntersect(const Vec3f &orig, const Vec3f &dir, const Triangle &tr);
float castRay(const Vec3f &origin, const Vec3f &dir, const Vec3f &lightSource, const std::vector<Triangle> &trs, int recursion);


void readTrianglesFromFile(std::string fileName, std::vector<Triangle> &trs, int amount=1) {
	std::ifstream inf(fileName);
	if (!inf)
	{
		std::cout << "Uh oh, file could not be opened for reading!" << std::endl;
		exit(1);
	}

	Vec3f p0, p1, p2;

	while (inf && amount--)
	{
		inf >> p0.x >> p0.y >> p0.z;
		inf >> p1.x >> p1.y >> p1.z;
		inf >> p2.x >> p2.y >> p2.z;
		trs.push_back(Triangle(p0, p1, p2));
	}
}

float castRay(const Vec3f &origin, const Vec3f &dir, const Vec3f &lightSource, const std::vector<Triangle> &trs, int recursion) {
	if (recursion >= DEPTH)
		return 0.0;

	recursion++;

	Intersection a;
	float ret = 0.0;
	for (int i = 0; i < trs.size(); i++) {
		a = rayTriangleIntersect(origin, dir, trs[i]);

		if (a.isIntersected) {
			switch (a.side) {
				case 1: { // semi-transparent
					float flare = powf(fabs(trs[i].normal * (a.intersectionPoint - lightSource).normalize()), FLARE_POW);
					ret += FLARE_COEFF * flare + LIGHT_COEFF * fabs(trs[i].normal * (a.intersectionPoint - lightSource).normalize()) + \
							castRay(a.intersectionPoint + dir * 0.001, dir, lightSource, trs, recursion);
					break;
				}
				case -1: { // semi-specular + semi-transparent
					float flare = powf(fabs(trs[i].normal * (a.intersectionPoint - lightSource).normalize()), FLARE_POW);
					ret += FLARE_COEFF * flare + LIGHT_COEFF * fabs(trs[i].normal * (a.intersectionPoint - lightSource).normalize()) + \
							0.5 * castRay(a.intersectionPoint + dir * 0.001, dir, lightSource, trs, recursion) + \
							0.5 * castRay(a.intersectionPoint - dir * 0.001, dir - trs[i].normal * 2.0 * (trs[i].normal * dir), lightSource, trs, recursion);
					break;
				}
			}
		}
	}
	return ret;
}

void saveImage(char* filename, int w, int h, unsigned char* data) {
	const int channels_num = 1;
	stbi_write_jpg(filename, w, h, channels_num, data, 100);
}

Intersection rayTriangleIntersect(const Vec3f &orig, const Vec3f &dir, const Triangle &tr) {
	float u, v, t;
	Vec3f v0v1 = tr.v1 - tr.v0;
    Vec3f v0v2 = tr.v2 - tr.v0;
    Vec3f pvec = cross(dir, v0v2);
    float det = v0v1 * pvec;
	Intersection ret;

    // if the determinant is close to 0, the ray misses the triangle
	if (fabs(det) < EPSILON)
		return ret;

    float invDet = 1 / det;

    Vec3f tvec = orig - tr.v0;
    u = tvec * pvec * invDet;
    if (u < 0 || u > 1)
    	return ret;

    Vec3f qvec = cross(tvec, v0v1);
    v = dir * qvec * invDet;
    if (v < 0 || u + v > 1)
    	return ret;

    t = v0v2 * qvec * invDet;
    if (t > 0.0) {
		ret.isIntersected = true;
		float s = (dir - orig) * tr.normal;
    	ret.side = s < 0.0 ? -1 : 1;
    	ret.intersectionPoint = orig + dir * t;
	}

	return ret;
}

#endif //RTX_RTX_H
