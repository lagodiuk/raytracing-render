#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <render.h>
#include <utils.h>

#include <canvas.h>

// Declarations
// --------------------------------------------------------------

// TriangleWithTexture extends Triangle3d
typedef
struct {
    Object3d * triangle;
    
    Point3d p1;
    Point3d p2;
    Point3d p3;
    
    Vector3d v_p1_p2;
    Vector3d v_p2_p3;
    Vector3d v_p3_p1;
    
    Point2d t1;
    Point2d t2;
    Point2d t3;
    
    Canvas * texture;
}
TriangleWithTexture;

inline static Boolean
intersect_tvt(const void * data,
              const Point3d vector_start,
              const Vector3d vector,
              Point3d * const intersection_point);

Point3d
get_min_tvt_boundary_point(const void * data);

Point3d
get_max_tvt_boundary_point(const void * data);

static inline Color
get_tvt_color(const void * data,
              const Point3d intersection_point);

static inline Vector3d
get_tvt_normal_vector(const void * data,
                      const Point3d intersection_point);

static inline Material
get_tvt_material(const void * data,
                 const Point3d intersection_point);

static inline void
release_tvt_data(void * data);

// Code
// --------------------------------------------------------------

Object3d *
new_triangle_with_texture(const Point3d p1,
                          const Point3d p2,
                          const Point3d p3,
                          const Point2d t1,
                          const Point2d t2,
                          const Point2d t3,
                          Canvas * texture,
                          const Color color,
                          const Material material) {
    
    // Create actual
    
    TriangleWithTexture * tvt = malloc(sizeof(TriangleWithTexture));
    
    // Create underlying triangle - "super class"
    tvt->triangle = new_triangle(p1, p2, p3, color, material);
    
    tvt->t1 = t1;
    tvt->t2 = t2;
    tvt->t3 = t3;
    tvt->texture = texture;
    
    tvt->p1 = p1;
    tvt->p2 = p2;
    tvt->p3 = p3;
    
    tvt->v_p1_p2 = vector3dp(p1, p2);
    normalize_vector(&tvt->v_p1_p2);
    
    tvt->v_p2_p3 = vector3dp(p2, p3);
    normalize_vector(&tvt->v_p2_p3);
    
    tvt->v_p3_p1 = vector3dp(p3, p1);
    normalize_vector(&tvt->v_p3_p1);
    
    // Wrap into Object3d structure
    
	Object3d * obj = malloc(sizeof(Object3d));
    obj->data = tvt;
	obj->release_data = release_tvt_data;
	obj->get_color = get_tvt_color;
	obj->intersect = intersect_tvt;
    obj->get_normal_vector = get_tvt_normal_vector;
    obj->get_material = get_tvt_material;
    obj->get_min_boundary_point = get_min_tvt_boundary_point;
    obj->get_max_boundary_point = get_max_tvt_boundary_point;
    
	return obj;
}

static inline void
release_tvt_data(void * data) {
    
    TriangleWithTexture * tvt = data;
    
	Object3d * triangle = tvt->triangle;
    triangle->release_data(triangle->data);
	free(triangle);
    
    free(tvt);
}

static inline Vector3d
get_tvt_normal_vector(const void * data,
                      const Point3d intersection_point) {
    
  	const TriangleWithTexture * tvt = data;
    Object3d * triangle = tvt->triangle;
    
    return triangle->get_normal_vector(triangle->data,
                                       intersection_point);
}

static inline Color
get_tvt_color(const void * data,
              const Point3d intersection_point) {
    
	const TriangleWithTexture * tvt = data;
    
    Vector3d v_p1_p = vector3dp(tvt->p1, intersection_point);
    Vector3d v_p2_p = vector3dp(tvt->p2, intersection_point);
    Vector3d v_p3_p = vector3dp(tvt->p3, intersection_point);
    
    Float h1 = module_vector(cross_product(v_p2_p, tvt->v_p2_p3));
    Float h2 = module_vector(cross_product(v_p3_p, tvt->v_p3_p1));
    Float h3 = module_vector(cross_product(v_p1_p, tvt->v_p1_p2));
    
    Float h_sum = h1 + h2 + h3;
    
    h1 /= h_sum;
    h2 /= h_sum;
    h3 /= h_sum;
    
    const Point2d t1 = tvt->t1;
    const Point2d t2 = tvt->t2;
    const Point2d t3 = tvt->t3;
    
    Float xf = h1 * t1.x + h2 * t2.x + h3 * t3.x;
    Float yf = h1 * t1.y + h2 * t2.y + h3 * t3.y;
    
    Canvas * canvas = tvt->texture;
    
    int x = (int)(xf * canvas->w) % canvas->w;
    int y = (int)(yf * canvas->h) % canvas->h;
    
    return get_pixel(x, y, canvas);
}

static inline Material
get_tvt_material(const void * data,
                 const Point3d intersection_point) {
    
    const TriangleWithTexture * tvt = data;
	const Object3d * triangle = tvt->triangle;
    
    return triangle->get_material(triangle->data, intersection_point);
}

Point3d
get_min_tvt_boundary_point(const void * data) {

    
    const TriangleWithTexture * tvt = data;
	const Object3d * triangle = tvt->triangle;
    
    return triangle->get_min_boundary_point(triangle->data);
}

Point3d
get_max_tvt_boundary_point(const void * data) {

    const TriangleWithTexture * tvt = data;
	const Object3d * triangle = tvt->triangle;
    
    return triangle->get_max_boundary_point(triangle->data);
}

inline static Boolean
intersect_tvt(const void * data,
              const Point3d vector_start,
              const Vector3d vector,
              Point3d * const intersection_point) {
    
    const TriangleWithTexture * tvt = data;
	const Object3d * triangle = tvt->triangle;
    
    return triangle->intersect(triangle->data, vector_start, vector, intersection_point);
}
