#include <math.h>
#include <render.h>
#include <utils.h>

inline Float module_vector3d(Vector3d v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Float cos_vectors3d(Vector3d v1, Vector3d v2) {
    Float numerator = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    Float denominator = module_vector3d(v1) * module_vector3d(v2);
    return numerator / denominator;
}

inline Float herons_square(Float a, Float b, Float c) {
    Float p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

inline Point3d rotate(Point3d p, Float sin_al, Float cos_al, Float sin_be, Float cos_be) {
	Float x = p.x * cos_al - p.y * sin_al;
	Float y = p.x * sin_al * cos_be + p.y * cos_al * cos_be - p.z * sin_be;
	Float z = p.x * sin_al * sin_be + p.y * cos_al * sin_be + p.z * cos_be;
	return point3d(x, y, z);
}