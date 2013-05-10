#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <render.h>
#include <utils.h>

// Declarations
// --------------------------------------------------------------

// TriangleWithNorms extends Triangle3d
typedef
struct {
    Object3d * triangle;
    
    Point3d p1;
    Point3d p2;
    Point3d p3;
    
    Vector3d n1;
    Vector3d n2;
    Vector3d n3;
    
    Vector3d v_p1_p2;
    Vector3d v_p2_p3;
    Vector3d v_p3_p1;
}
TriangleWithNorms;

inline static Boolean
intersect_tvn(const void * data,
              const Point3d vector_start,
              const Vector3d vector,
              Point3d * const intersection_point);

Point3d
get_min_tvn_boundary_point(const void * data);

Point3d
get_max_tvn_boundary_point(const void * data);

static inline Color
get_tvn_color(const void * data,
              const Point3d intersection_point);

static inline Vector3d
get_tvn_normal_vector(const void * data,
                      const Point3d intersection_point);

static inline Material
get_tvn_material(const void * data,
                 const Point3d intersection_point);

static inline void
release_tvn_data(void * data);

// Code
// --------------------------------------------------------------

Object3d *
new_triangle_with_norms(const Point3d p1,
                        const Point3d p2,
                        const Point3d p3,
                        const Vector3d n1,
                        const Vector3d n2,
                        const Vector3d n3,
                        const Color color,
                        const Material material) {
    
    // Create actual
    
    TriangleWithNorms * tvn = malloc(sizeof(TriangleWithNorms));
    
    // Create underlying triangle - "super class"
    tvn->triangle = new_triangle(p1, p2, p3, color, material);
    
    tvn->n1 = n1;
    tvn->n2 = n2;
    tvn->n3 = n3;
    
    tvn->p1 = p1;
    tvn->p2 = p2;
    tvn->p3 = p3;
    
    tvn->v_p1_p2 = vector3dp(p1, p2);
    normalize_vector(&tvn->v_p1_p2);
    
    tvn->v_p2_p3 = vector3dp(p2, p3);
    normalize_vector(&tvn->v_p2_p3);
    
    tvn->v_p3_p1 = vector3dp(p3, p1);
    normalize_vector(&tvn->v_p3_p1);
    
    // Wrap into Object3d structure
    
	Object3d * obj = malloc(sizeof(Object3d));
    obj->data = tvn;
	obj->release_data = release_tvn_data;
	obj->get_color = get_tvn_color;
	obj->intersect = intersect_tvn;
    obj->get_normal_vector = get_tvn_normal_vector;
    obj->get_material = get_tvn_material;
    obj->get_min_boundary_point = get_min_tvn_boundary_point;
    obj->get_max_boundary_point = get_max_tvn_boundary_point;
    
	return obj;
}

static inline void
release_tvn_data(void * data) {
    
    TriangleWithNorms * tvn = data;
    
	Object3d * triangle = tvn->triangle;
    triangle->release_data(triangle->data);
	free(triangle);
    
    free(tvn);
}

static inline Vector3d
get_tvn_normal_vector(const void * data,
                      const Point3d intersection_point) {
    
  	const TriangleWithNorms * tvn = data;
    
    Vector3d v_p1_p = vector3dp(tvn->p1, intersection_point);
    Vector3d v_p2_p = vector3dp(tvn->p2, intersection_point);
    Vector3d v_p3_p = vector3dp(tvn->p3, intersection_point);
    
    Float h1 = module_vector(cross_product(v_p2_p, tvn->v_p2_p3));
    Float h2 = module_vector(cross_product(v_p3_p, tvn->v_p3_p1));
    Float h3 = module_vector(cross_product(v_p1_p, tvn->v_p1_p2));
    
    Float h_sum = h1 + h2 + h3;
    
    h1 /= h_sum;
    h2 /= h_sum;
    h3 /= h_sum;
    
    return vector3df(h1 * tvn->n1.x + h2 * tvn->n2.x + h3 * tvn->n3.x,
                     h1 * tvn->n1.y + h2 * tvn->n2.y + h3 * tvn->n3.y,
                     h1 * tvn->n1.z + h2 * tvn->n2.z + h3 * tvn->n3.z);
}

static inline Color
get_tvn_color(const void * data,
              const Point3d intersection_point) {
    
	const TriangleWithNorms * tvn = data;
	const Object3d * triangle = tvn->triangle;
    
    return triangle->get_color(triangle->data, intersection_point);
}

static inline Material
get_tvn_material(const void * data,
                 const Point3d intersection_point) {
    
    const TriangleWithNorms * tvn = data;
	const Object3d * triangle = tvn->triangle;
    
    return triangle->get_material(triangle->data, intersection_point);
}

Point3d
get_min_tvn_boundary_point(const void * data) {

    
    const TriangleWithNorms * tvn = data;
	const Object3d * triangle = tvn->triangle;
    
    return triangle->get_min_boundary_point(triangle->data);
}

Point3d
get_max_tvn_boundary_point(const void * data) {

    const TriangleWithNorms * tvn = data;
	const Object3d * triangle = tvn->triangle;
    
    return triangle->get_max_boundary_point(triangle->data);
}

inline static Boolean
intersect_tvn(const void * data,
              const Point3d vector_start,
              const Vector3d vector,
              Point3d * const intersection_point) {
    
    const TriangleWithNorms * tvn = data;
	const Object3d * triangle = tvn->triangle;
    
    return triangle->intersect(triangle->data, vector_start, vector, intersection_point);
}
