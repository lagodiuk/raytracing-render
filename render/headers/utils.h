#ifndef UTILS_3D_H
#define UTILS_3D_H

#include "render.h"
#include <math.h>

#define EPSILON 1e-5

static inline Float module_vector(Vector3d v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Float cos_vectors(Vector3d v1, Vector3d v2) {
    Float numerator = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    Float denominator = module_vector(v1) * module_vector(v2);
    return numerator / denominator;
}

static inline void normalize_vector(Vector3d * v) {
    Float module = module_vector(*v);
    v->x = v->x / module;
    v->y = v->y / module;
    v->z = v->z / module;
}

static inline Float herons_square(Float a, Float b, Float c) {
    Float p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

static inline Point3d rotate_point(Point3d p, Float sin_al, Float cos_al, Float sin_be, Float cos_be) {
	Float x = p.x * cos_al - p.y * sin_al;
	Float y = p.x * sin_al * cos_be + p.y * cos_al * cos_be - p.z * sin_be;
	Float z = p.x * sin_al * sin_be + p.y * cos_al * sin_be + p.z * cos_be;
	return point3d(x, y, z);
}

#endif
