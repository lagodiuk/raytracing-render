#ifndef UTILS_3D_H
#define UTILS_3D_H

#include <render.h>
#include <math.h>

static inline Vector3d
cross_product(const Vector3d a,
              const Vector3d b) {
    
    return vector3df(a.z * b.y - a.y * b.z,
                     a.x * b.z - a.z * b.x,
                     a.y * b.x - a.x * b.y);
}

static inline Float
dot_product(const Vector3d v1,
            const Vector3d v2) {
    
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static inline Float
sqr_module_vector(const Vector3d v) {
    return dot_product(v, v);
}

static inline Float
module_vector(const Vector3d v) {
    return sqrt(sqr_module_vector(v));
}

static inline Float
cos_vectors(const Vector3d v1,
            const Vector3d v2) {
    
    return dot_product(v1, v2) / sqrt(sqr_module_vector(v1) * sqr_module_vector(v2));
}

static inline void
normalize_vector(Vector3d * const v) {
    const Float module = module_vector(*v);
    v->x = v->x / module;
    v->y = v->y / module;
    v->z = v->z / module;
}

static inline Vector3d
reflect_ray(const Vector3d incident_ray,
            const Vector3d norm_v) {
    
    const Float k = 2 * dot_product(incident_ray, norm_v) / sqr_module_vector(norm_v);
    
    const Float x = incident_ray.x - norm_v.x * k;
    const Float y = incident_ray.y - norm_v.y * k;
    const Float z = incident_ray.z - norm_v.z * k;
    
    return vector3df(x, y, z);
}

// TODO: use matrixes
static inline Point3d
rotate_point_x(const Point3d p,
               const Float sin_al,
               const Float cos_al) {
    
	const Float y = p.y * cos_al - p.z * sin_al;
	const Float z = p.y * sin_al + p.z * cos_al;
    
	return point3d(p.x, y, z);
}

static inline Point3d
rotate_point_y(const Point3d p,
               const Float sin_al,
               const Float cos_al) {
    
	const Float x = p.x * cos_al - p.z * sin_al;
	const Float z = p.x * sin_al + p.z * cos_al;
    
	return point3d(x, p.y, z);
}

static inline Point3d
rotate_point_z(const Point3d p,
               const Float sin_al,
               const Float cos_al) {
    
	const Float x = p.x * cos_al - p.y * sin_al;
	const Float y = p.x * sin_al + p.y * cos_al;
    
	return point3d(x, y, p.z);
}

// TODO: use matrixes
static inline Vector3d
rotate_vector_x(const Vector3d p,
                const Float sin_al,
                const Float cos_al) {
    
	const Float y = p.y * cos_al - p.z * sin_al;
	const Float z = p.y * sin_al + p.z * cos_al;
    
	return vector3df(p.x, y, z);
}

static inline Vector3d
rotate_vector_y(const Vector3d p,
                const Float sin_al,
                const Float cos_al) {
    
	const Float x = p.x * cos_al - p.z * sin_al;
	const Float z = p.x * sin_al + p.z * cos_al;
    
	return vector3df(x, p.y, z);
}

static inline Vector3d
rotate_vector_z(const Vector3d p,
                const Float sin_al,
                const Float cos_al) {
    
	const Float x = p.x * cos_al - p.y * sin_al;
	const Float y = p.x * sin_al + p.y * cos_al;
    
	return vector3df(x, y, p.z);
}
#endif
