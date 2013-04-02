#ifndef UTILS_3D_H
#define UTILS_3D_H

#include <render.h>

#define EPSILON 1e-5

inline Float cos_vectors3d(Vector3d v1, Vector3d v2);

inline Float module_vector3d(Vector3d v);

inline Float herons_square(Float a, Float b, Float c);

inline Point3d rotate(Point3d p, Float sin_al, Float cos_al, Float sin_be, Float cos_be);

#endif