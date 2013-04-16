#ifndef UTILS_3D_H
#define UTILS_3D_H

#include "render.h"
#include <math.h>

#define EPSILON 1e-5

static inline Float
module_vector(const Vector3d v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Float
sqr_module_vector(const Vector3d v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline Float
cos_vectors(const Vector3d v1,
            const Vector3d v2) {
    
    Float numerator = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    Float denominator = module_vector(v1) * module_vector(v2);
    return numerator / denominator;
}

static inline void
normalize_vector(Vector3d * const v) {
    Float module = module_vector(*v);
    v->x = v->x / module;
    v->y = v->y / module;
    v->z = v->z / module;
}

static inline Point3d
rotate_point(const Point3d p,
             const Float sin_al,
             const Float cos_al,
             const Float sin_be,
             const Float cos_be) {

	Float x = p.z * sin_al * sin_be + p.y * cos_al * sin_be + p.x * cos_be;
	Float y = p.z * sin_al * cos_be + p.y * cos_al * cos_be - p.x * sin_be;
	Float z = p.z * cos_al - p.y * sin_al;
    
	return point3d(x, y, z);
}

static inline Vector3d
rotate_vector(const Vector3d v,
             const Float sin_al,
             const Float cos_al,
             const Float sin_be,
             const Float cos_be) {
    

    Float x = v.z * sin_al * sin_be + v.y * cos_al * sin_be + v.x * cos_be;
	Float y = v.z * sin_al * cos_be + v.y * cos_al * cos_be - v.x * sin_be;
	Float z = v.z * cos_al - v.y * sin_al;
    
	return vector3df(x, y, z);
}

#endif
