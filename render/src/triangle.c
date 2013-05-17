#include <math.h>
#include <stdlib.h>

#include <render.h>
#include <utils.h>

// Declarations
// --------------------------------------------------------------

typedef
struct {
    /************
     * Geometry *
     ************/
    
    // Absolute (world) vertexes of triangle
	Point3d p1;
	Point3d p2;
	Point3d p3;

    Vector3d norm;
    
    // distance to the center of coordinates
	Float d;
    
    Vector3d v_p1_p2;
    Vector3d v_p2_p3;
    Vector3d v_p3_p1;
    
    /************
     * Material *
     ************/
    
    Color color;
    Material material;
}
Triangle3d;

inline static Boolean
intersect_triangle(const void * data,
                   const Point3d vector_start,
                   const Vector3d vector,
                   Point3d * const intersection_point);

Point3d
get_min_triangle_boundary_point(const void * data);

Point3d
get_max_triangle_boundary_point(const void * data);

static inline Color
get_triangle_color(const void * data,
                   const Point3d intersection_point);

static inline Vector3d
get_triangle_normal_vector(const void * data,
                           const Point3d intersection_point);

static inline Material
get_triangle_material(const void * data,
                      const Point3d intersection_point);

static inline void
release_triangle_data(void * data);

static inline Boolean
check_same_clock_dir(const Vector3d v1,
                     const Vector3d v2,
                     const Vector3d norm);

// Code
// --------------------------------------------------------------

Object3d *
new_triangle(const Point3d p1,
             const Point3d p2,
             const Point3d p3,
             const Color color,
             const Material material) {
    
	Triangle3d * triangle = malloc(sizeof(Triangle3d));
    
	triangle->p1 = p1;
	triangle->p2 = p2;
	triangle->p3 = p3;
    
    triangle->norm = cross_product(vector3dp(p1, p3), vector3dp(p3, p2));
    
	triangle->d = -(p1.x * triangle->norm.x + p1.y * triangle->norm.y + p1.z * triangle->norm.z);
    
    triangle->color = color;
    triangle->material = material;
    
    triangle->v_p1_p2 = vector3dp(p1, p2);
    triangle->v_p2_p3 = vector3dp(p2, p3);
    triangle->v_p3_p1 = vector3dp(p3, p1);
    
	Object3d * obj = malloc(sizeof(Object3d));
	obj->data = triangle;
	obj->release_data = release_triangle_data;
	obj->get_color = get_triangle_color;
	obj->intersect = intersect_triangle;
    obj->get_normal_vector = get_triangle_normal_vector;
    obj->get_material = get_triangle_material;
    obj->get_min_boundary_point = get_min_triangle_boundary_point;
    obj->get_max_boundary_point = get_max_triangle_boundary_point;
    
	return obj;
}

static inline Color
get_triangle_color(const void * data,
                   const Point3d intersection_point) {
	const Triangle3d * triangle = data;
	return triangle->color;
}

static inline Vector3d
get_triangle_normal_vector(const void * data,
                           const Point3d intersection_point) {
  	const Triangle3d * triangle = data;
    return triangle->norm;
}

static inline Material
get_triangle_material(const void * data,
                      const Point3d intersection_point) {
    const Triangle3d * triangle = data;
    return triangle->material;
}

static inline void
release_triangle_data(void * data) {
	Triangle3d * triangle = data;
	free(triangle);
}

Point3d
get_min_triangle_boundary_point(const void * data) {
	const Triangle3d * t = data;
    
    Float x_min = t->p1.x;
    Float y_min = t->p1.y;
    Float z_min = t->p1.z;
    
    x_min = (x_min < t->p2.x) ? x_min : t->p2.x;
    y_min = (y_min < t->p2.y) ? y_min : t->p2.y;
    z_min = (z_min < t->p2.z) ? z_min : t->p2.z;
    
    x_min = (x_min < t->p3.x) ? x_min : t->p3.x;
    y_min = (y_min < t->p3.y) ? y_min : t->p3.y;
    z_min = (z_min < t->p3.z) ? z_min : t->p3.z;
    
    return point3d(x_min - EPSILON, y_min - EPSILON, z_min - EPSILON);
}

Point3d
get_max_triangle_boundary_point(const void * data) {
	const Triangle3d * t = data;
    
    Float x_max = t->p1.x;
    Float y_max = t->p1.y;
    Float z_max = t->p1.z;
    
    x_max = (x_max > t->p2.x) ? x_max : t->p2.x;
    y_max = (y_max > t->p2.y) ? y_max : t->p2.y;
    z_max = (z_max > t->p2.z) ? z_max : t->p2.z;
    
    x_max = (x_max > t->p3.x) ? x_max : t->p3.x;
    y_max = (y_max > t->p3.y) ? y_max : t->p3.y;
    z_max = (z_max > t->p3.z) ? z_max : t->p3.z;
    
    return point3d(x_max + EPSILON, y_max + EPSILON, z_max + EPSILON);
}

inline static Boolean
intersect_triangle(const void * data,
                   const Point3d vector_start,
                   const Vector3d vector,
                   Point3d * const intersection_point) {
    
	const Triangle3d * tr = data;
    
    const Float scalar_product = dot_product(tr->norm, vector);
    
    if(fabs(scalar_product) < EPSILON) {
        // Ray is perpendicular to triangles normal vector (A, B, C)
        // it means that ray is parellel to triangle
        // so there is no intersection
        return False;
    }
    
	const Float k = - (tr->norm.x * vector_start.x
                 + tr->norm.y * vector_start.y
                 + tr->norm.z * vector_start.z
                 + tr->d)
            / scalar_product;
    
    if(k < EPSILON) {
        // Avoid intersection in the opposite direction
        return False;
    }
	
    // Intersection point
	const Float x = vector_start.x + vector.x * k;
	const Float y = vector_start.y + vector.y * k;
	const Float z = vector_start.z + vector.z * k;
	const Point3d ipt = point3d(x, y, z);
    
    if(check_same_clock_dir(tr->v_p1_p2, vector3dp(tr->p1, ipt), tr->norm)
       && check_same_clock_dir(tr->v_p2_p3, vector3dp(tr->p2, ipt), tr->norm)
       && check_same_clock_dir(tr->v_p3_p1, vector3dp(tr->p3, ipt), tr->norm)) {

        *intersection_point = ipt;
        return True;
    }
    
    // No intersection
	return False;
}

static inline Boolean
check_same_clock_dir(const Vector3d v1,
                     const Vector3d v2,
                     const Vector3d norm) {
    
    const Vector3d norm_v1_v2 = cross_product(v2, v1);
    
    if(dot_product(norm_v1_v2, norm) < 0)
        return False;
    else
        return True;
}
