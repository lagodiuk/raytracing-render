#include <math.h>
#include <stdlib.h>

#include <render.h>
#include <utils.h>

// Declarations
// --------------------------------------------------------------

typedef
struct {
    Point3d center;
    Float radius;
    Color color;
    Material material;
}
Sphere;

static Boolean
intersect_sphere(const void * data,
                 const Point3d vector_start,
                 const Vector3d vector,
                 Point3d * const intersection_point);

static Point3d
get_min_sphere_boundary_point(const void * data);

static Point3d
get_max_sphere_boundary_point(const void * data);

static Color
get_sphere_color(const void * data,
                 const Point3d intersection_point);

static Vector3d
get_sphere_normal_vector(const void * data,
                         const Point3d intersection_point);

static Material
get_sphere_material(const void * data,
                    const Point3d intersection_point);

void
release_sphere_data(void * data);

// Code
// --------------------------------------------------------------

Object3d *
new_sphere(const Point3d center,
           const Float radius,
           const Color color,
           const Material material) {
    
    Sphere * sphere = malloc(sizeof(Sphere));
    sphere->center = center;
    sphere->radius = radius;
    sphere->color = color;
    sphere->material = material;
    
    Object3d * obj = malloc(sizeof(Object3d));
    obj->data = sphere;
    obj->release_data = release_sphere_data;
	obj->get_color = get_sphere_color;
	obj->intersect = intersect_sphere;
    obj->get_normal_vector = get_sphere_normal_vector;
    obj->get_material = get_sphere_material;
    obj->get_min_boundary_point = get_min_sphere_boundary_point;
    obj->get_max_boundary_point = get_max_sphere_boundary_point;
    
    return obj;
}

static Color
get_sphere_color(const void * data,
                 const Point3d intersection_point) {
	const Sphere * sphere = data;
	return sphere->color;
}

static Vector3d
get_sphere_normal_vector(const void * data,
                         const Point3d intersection_point) {
  	const Sphere * sphere = data;    
    Vector3d n = vector3dp(sphere->center, intersection_point);
    normalize_vector(&n);
    return n;
}

static Material
get_sphere_material(const void * data,
                      const Point3d intersection_point) {
    const Sphere * sphere = data;
    return sphere->material;
}

void
release_sphere_data(void * data) {
	Sphere * sphere = data;
	free(sphere);
}

Point3d
get_min_sphere_boundary_point(const void * data) {
	const Sphere * sphere = data;
    const Point3d c = sphere->center;
    const Float r = sphere->radius;
    return point3d(c.x - r - 1, c.y - r - 1, c.z - r - 1);
}

Point3d
get_max_sphere_boundary_point(const void * data) {
	const Sphere * sphere = data;
    const Point3d c = sphere->center;
    const Float r = sphere->radius;
    return point3d(c.x + r + 1, c.y + r + 1, c.z + r + 1);
}

static Boolean
intersect_sphere(const void * data,
                 const Point3d vector_start,
                 const Vector3d vector,
                 Point3d * const intersection_point) {
    
    const Sphere * sphere = data;
    
    const Point3d center = sphere->center;
    const Float r = sphere->radius;
    
    const Float a = vector.x * vector.x
                    + vector.y * vector.y
                    + vector.z * vector.z;
    
    const Float b = 2 * (vector.x * (vector_start.x - center.x)
                         + vector.y * (vector_start.y - center.y)
                         + vector.z * (vector_start.z - center.z));
    
    const Float c = center.x * center.x
                    + center.y * center.y
                    + center.z * center.z
                    + vector_start.x * vector_start.x
                    + vector_start.y * vector_start.y
                    + vector_start.z * vector_start.z
                    - 2 * (vector_start.x * center.x
                           + vector_start.y * center.y
                           + vector_start.z * center.z)
                    - r * r;
    
    const Float D = b * b - 4 * a * c;
    
    if(D < 0)
        return False;
    
    const Float sqrt_D = sqrt(D);
    const Float a_2 = 2 * a;
    
    const Float t1 = (-b + sqrt_D) / a_2;
    const Float t2 = (-b - sqrt_D) / a_2;
    
    const Float min_t = (t1 < t2) ? t1 : t2;
    const Float max_t = (t1 > t2) ? t1 : t2;
    
    const Float t = (min_t > EPSILON) ? min_t : max_t;
    
    if(t < EPSILON)
        return False;
    
    *intersection_point = point3d(vector_start.x + t * vector.x,
                                  vector_start.y + t * vector.y,
                                  vector_start.z + t * vector.z);
    
    return True;
}