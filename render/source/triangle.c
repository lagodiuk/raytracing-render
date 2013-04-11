#include <math.h>
#include <stdlib.h>
#include <render.h>
#include <utils.h>

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
    // Projection vertexes of triangle
	Point3d p1;
	Point3d p2;
	Point3d p3;
    // Projection norm vector (A, B, C)
    // A * x + B * y + C * z + D = 0
	Float A;
	Float B;
	Float C;
	Float D;
    // Pre-calculated lengths of the sides of a triangle
    Float d_p1_p2;
    Float d_p2_p3;
    Float d_p3_p1;
    // Pre-calculated square of triangle
    Float s;
    
    /************
     * Material *
     ************/
    
    Color color;

    Material material;
}
Triangle3d;


static inline Color get_triangle_color(void * data,
                                       Point3d intersection_point) {
	Triangle3d * triangle = data;
	return triangle->color;
}

static inline Vector3d get_triangle_normal_vector(void * data,
                                                  Point3d intersection_point) {
  	Triangle3d * triangle = data;
    return vector3df(triangle->A, triangle->B, triangle->C);
}

static inline Material get_triangle_material(void * data,
                                             Point3d intersection_point) {
    Triangle3d * triangle = data;
    return triangle->material;
}

static inline void release_triangle_data(void * data) {
	Triangle3d * triangle = data;
	free(triangle);
}


void rotate_triangle(void * data,
                     Float sin_al,
                     Float cos_al,
                     Float sin_be,
                     Float cos_be);

Color get_triangle_color(void * data,
                         Point3d intersection_point);

int intersect_triangle(void * data,
                       Point3d vector_start,
                       Vector3d vector,
                       Point3d * intersection_point);


Point3d get_min_triangle_boundary_point(void * data);

Point3d get_max_triangle_boundary_point(void * data);


Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3, Color color, Material material) {
	Triangle3d * triangle = malloc(sizeof(Triangle3d));
	triangle->p1w = p1;
	triangle->p2w = p2;
	triangle->p3w = p3;
	triangle->Aw = (p1.y - p3.y) * (p2.z - p3.z) - (p1.z - p3.z) * (p2.y - p3.y);
	triangle->Bw = (p2.x - p3.x) * (p1.z - p3.z) - (p2.z - p3.z) * (p1.x - p3.x);
	triangle->Cw = (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
	triangle->Dw = -(p1.x * triangle->Aw + p1.y * triangle->Bw + p1.z * triangle->Cw);
    triangle->d_p1_p2 = module_vector(vector3dp(p1, p2));
    triangle->d_p2_p3 = module_vector(vector3dp(p2, p3));
    triangle->d_p3_p1 = module_vector(vector3dp(p3, p1));
    triangle->s = herons_square(triangle->d_p1_p2, triangle->d_p2_p3, triangle->d_p3_p1);
    triangle->color = color;
    triangle->material = material;
    
	Object3d * obj = malloc(sizeof(Object3d));
	obj->data = triangle;
	obj->rotate = rotate_triangle;
	obj->release_data = release_triangle_data;
	obj->get_color = get_triangle_color;
	obj->intersect = intersect_triangle;
    obj->get_normal_vector = get_triangle_normal_vector;
    obj->get_material = get_triangle_material;
    obj->get_min_boundary_point = get_min_triangle_boundary_point;
    obj->get_max_boundary_point = get_max_triangle_boundary_point;
    
	return obj;
}

Point3d get_min_triangle_boundary_point(void * data) {
	Triangle3d * t = data;
    
    Float x_min = t->p1.x;
    Float y_min = t->p1.y;
    Float z_min = t->p1.z;
    
    x_min = (x_min < t->p2.x) ? x_min : t->p2.x;
    y_min = (y_min < t->p2.y) ? y_min : t->p2.y;
    z_min = (z_min < t->p2.z) ? z_min : t->p2.z;
    
    x_min = (x_min < t->p3.x) ? x_min : t->p3.x;
    y_min = (y_min < t->p3.y) ? y_min : t->p3.y;
    z_min = (z_min < t->p3.z) ? z_min : t->p3.z;
    
    return point3d(x_min - 1, y_min - 1, z_min - 1);
}

Point3d get_max_triangle_boundary_point(void * data) {
	Triangle3d * t = data;
    
    Float x_max = t->p1.x;
    Float y_max = t->p1.y;
    Float z_max = t->p1.z;
    
    x_max = (x_max > t->p2.x) ? x_max : t->p2.x;
    y_max = (y_max > t->p2.y) ? y_max : t->p2.y;
    z_max = (z_max > t->p2.z) ? z_max : t->p2.z;
    
    x_max = (x_max > t->p3.x) ? x_max : t->p3.x;
    y_max = (y_max > t->p3.y) ? y_max : t->p3.y;
    z_max = (z_max > t->p3.z) ? z_max : t->p3.z;
    
    return point3d(x_max + 1, y_max + 1, z_max + 1);
}

void rotate_triangle(void * data, Float sin_al, Float cos_al, Float sin_be, Float cos_be) {
	Triangle3d * triangle = data;
    
	triangle->p1 = rotate_point(triangle->p1w, sin_al, cos_al, sin_be, cos_be);
	triangle->p2 = rotate_point(triangle->p2w, sin_al, cos_al, sin_be, cos_be);
	triangle->p3 = rotate_point(triangle->p3w, sin_al, cos_al, sin_be, cos_be);
    
	Point3d norm = rotate_point(point3d(triangle->Aw, triangle->Bw, triangle->Cw), sin_al, cos_al, sin_be, cos_be);
	triangle->A = norm.x;
	triangle->B = norm.y;
	triangle->C = norm.z;
	triangle->D = -(triangle->p1.x * triangle->A + triangle->p1.y * triangle->B + triangle->p1.z * triangle->C);
}

int intersect_triangle(void * data, Point3d vector_start, Vector3d vector, Point3d * intersection_point) {
	Triangle3d * tr = data;
    
    Float scalar_product = tr->A * vector.x + tr->B * vector.y + tr->C * vector.z;
    
    if(abs(scalar_product) < EPSILON) {
        // Ray is perpendicular to triangles normal vector (A, B, C)
        // it means that ray is parellel to triangle
        // so there is no intersection
        return False;
    }
    
	Float k = - (tr->A * vector_start.x
                 + tr->B * vector_start.y
                 + tr->C * vector_start.z
                 + tr->D)
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
    
    // Checking if point "ipt" is inside of triangle "p1-p2-p3"
    // using herons square formula:
    // point is inside when: S(p1-p2-ipt) + S(p2-p3-ipt) + S(p1-p3-ipt) = S(p1-p2-p3)
    
    // Calculating length of the sides: p1-ipt, p2-ipt, p3-ipt
	Float d_p1_ipt = module_vector(vector3dp(tr->p1, ipt));
	Float d_p2_ipt = module_vector(vector3dp(tr->p2, ipt));
	Float d_p3_ipt = module_vector(vector3dp(tr->p3, ipt));
    // length of other sides are pre-calculated:
    // p1-p2 is tr->d_p1_p2
    // p2-p3 is tr->d_p2_p3
    // p3-p1 is tr->d_p3_p1
    
    // Calculating S(p1-p2-ipt), S(p2-p3-ipt) and S(p1-p3-ipt)
    Float s1 = herons_square(tr->d_p1_p2, d_p1_ipt, d_p2_ipt);
    Float s2 = herons_square(tr->d_p2_p3, d_p2_ipt, d_p3_ipt);
    Float s3 = herons_square(tr->d_p3_p1, d_p3_ipt, d_p1_ipt);
    // Square of triangle p1-p2-p3 is pre-calculated too
    // S(p1-p2-p3) is tr->s
    
    if(abs(s1 + s2 + s3 - tr->s) < EPSILON) {
        // Triangle is intersected
        *intersection_point = ipt;
        return True;
    }
    
    // No intersection
	return False;
}

