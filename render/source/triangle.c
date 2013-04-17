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
	Point3d p1w;
	Point3d p2w;
	Point3d p3w;
    // Absolute (world) norm vector (Aw, Bw, Cw) of triangle
    // Aw * x + Bw * y + Cw * z + D = 0
	Float Aw;
	Float Bw;
	Float Cw;
	Float Dw;
    
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
check_same_clock_dir(const Point3d pt1,
                     const Point3d pt2,
                     const Point3d pt3,
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
    
	triangle->p1w = p1;
	triangle->p2w = p2;
	triangle->p3w = p3;
	triangle->Aw = (p1.y - p3.y) * (p2.z - p3.z) - (p1.z - p3.z) * (p2.y - p3.y);
	triangle->Bw = (p2.x - p3.x) * (p1.z - p3.z) - (p2.z - p3.z) * (p1.x - p3.x);
	triangle->Cw = (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
	triangle->Dw = -(p1.x * triangle->Aw + p1.y * triangle->Bw + p1.z * triangle->Cw);
    triangle->color = color;
    triangle->material = material;
    
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
    return vector3df(triangle->Aw, triangle->Bw, triangle->Cw);
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
    
    Float x_min = t->p1w.x;
    Float y_min = t->p1w.y;
    Float z_min = t->p1w.z;
    
    x_min = (x_min < t->p2w.x) ? x_min : t->p2w.x;
    y_min = (y_min < t->p2w.y) ? y_min : t->p2w.y;
    z_min = (z_min < t->p2w.z) ? z_min : t->p2w.z;
    
    x_min = (x_min < t->p3w.x) ? x_min : t->p3w.x;
    y_min = (y_min < t->p3w.y) ? y_min : t->p3w.y;
    z_min = (z_min < t->p3w.z) ? z_min : t->p3w.z;
    
    return point3d(x_min - 1, y_min - 1, z_min - 1);
}

Point3d
get_max_triangle_boundary_point(const void * data) {
	const Triangle3d * t = data;
    
    Float x_max = t->p1w.x;
    Float y_max = t->p1w.y;
    Float z_max = t->p1w.z;
    
    x_max = (x_max > t->p2w.x) ? x_max : t->p2w.x;
    y_max = (y_max > t->p2w.y) ? y_max : t->p2w.y;
    z_max = (z_max > t->p2w.z) ? z_max : t->p2w.z;
    
    x_max = (x_max > t->p3w.x) ? x_max : t->p3w.x;
    y_max = (y_max > t->p3w.y) ? y_max : t->p3w.y;
    z_max = (z_max > t->p3w.z) ? z_max : t->p3w.z;
    
    return point3d(x_max + 1, y_max + 1, z_max + 1);
}

inline static Boolean
intersect_triangle(const void * data,
                   const Point3d vector_start,
                   const Vector3d vector,
                   Point3d * const intersection_point) {
    
	const Triangle3d * tr = data;
    
    Float scalar_product = tr->Aw * vector.x + tr->Bw * vector.y + tr->Cw * vector.z;
    
    if(fabs(scalar_product) < EPSILON) {
        // Ray is perpendicular to triangles normal vector (A, B, C)
        // it means that ray is parellel to triangle
        // so there is no intersection
        return False;
    }
    
	Float k = - (tr->Aw * vector_start.x
                 + tr->Bw * vector_start.y
                 + tr->Cw * vector_start.z
                 + tr->Dw)
            / scalar_product;
    
    if(k < EPSILON) {
        // Avoid intersection in the opposite direction
        return False;
    }
	
	Float x = vector_start.x + vector.x * k;
	Float y = vector_start.y + vector.y * k;
	Float z = vector_start.z + vector.z * k;
    
    // Intersection point
	Point3d ipt = point3d(x, y, z);
    
    Vector3d norm = vector3df(tr->Aw, tr->Bw, tr->Cw);
    if(check_same_clock_dir(tr->p1w, tr->p2w, ipt, norm)
       && check_same_clock_dir(tr->p2w, tr->p3w, ipt, norm)
       && check_same_clock_dir(tr->p3w, tr->p1w, ipt, norm)) {

        *intersection_point = ipt;
        return True;
    }
    
    // No intersection
	return False;
}

static inline Boolean
check_same_clock_dir(const Point3d p1,
                     const Point3d p2,
                     const Point3d p3,
                     const Vector3d norm) {
    
    Float testi;
    Float testj;
    Float testk;
    
    Float dotprod;
    
    // normal of trinagle
    Float p1p3_x = p1.x - p3.x;
    Float p2p3_x = p2.x - p3.x;
    
    Float p1p3_y = p1.y - p3.y;
    Float p2p3_y = p2.y - p3.y;
    
    Float p1p3_z = p1.z - p3.z;
    Float p2p3_z = p2.z - p3.z;
    
    testi = p1p3_y * p2p3_z - p1p3_z * p2p3_y;
    testj = p2p3_x * p1p3_z - p2p3_z * p1p3_x;
    testk = p1p3_x * p2p3_y - p1p3_y * p2p3_x;
    
    // Dot product with triangle normal
    dotprod = testi * norm.x + testj * norm.y + testk * norm.z;
    
    //answer
    if(dotprod < 0)
        return False;
    else
        return True;
}
