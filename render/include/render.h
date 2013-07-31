#ifndef __RENDER_H__
#define __RENDER_H__

#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <color.h>
#include <canvas.h>

#ifdef RAY_INTERSECTIONS_STAT
long
intersections_per_ray;
#endif // RAY_INTERSECTIONS_STAT

typedef
int
Boolean;

#define True 1
#define False 0

// Using double for satisfactory accuracy
typedef
double
Float;

#define EPSILON 1e-5

#define FLOAT_MAX DBL_MAX

typedef
struct {
    Float x;
    Float y;
}
Point2d;

typedef 
struct {
	Float x;
	Float y;
	Float z;
}
Point3d;

typedef
struct {
	Float x;
	Float y;
	Float z;
}
Vector3d;

typedef
struct {
    // Absolute location
    Point3d location_world;
    // Location on projection
	Point3d location;
	Color color;
}
LightSource3d;

typedef
struct {
    // Required:
    // Ka + Kd + Ks + Kr + Kt = 1.0
    
    // Ambient
    Float Ka;
    // Diffuse
    Float Kd;
    // Specular
    Float Ks;
    // Reflection
    Float Kr;
    // Transparency
    Float Kt;
    
    // Ks * light_source_color * ((cos(..))^p)
    Float p;
}
Material;

typedef 
struct {
	void * data;
    
	Boolean (*intersect)(const void * data,
                         const Point3d vector_start,
                         const Vector3d vector,
                         Point3d * const intersection_point);
    
	Color (*get_color)(const void * data,
                       const Point3d intersection_point);
    
    Vector3d (*get_normal_vector)(const void * data,
                                  const Point3d intersection_point);
    
    Material (*get_material)(const void * data,
                             const Point3d intersection_point);
    
    Point3d (*get_min_boundary_point)(const void * data);
    
    Point3d (*get_max_boundary_point)(const void * data);
    
	void (*release_data)(void * data);
}
Object3d;

// KD Tree
enum Plane {XY, XZ, YZ, NONE};

typedef
union {
    Float x;
    Float y;
    Float z;
}
Coord;

typedef
struct {
    Float x_min;
    Float y_min;
    Float z_min;
    
    Float x_max;
    Float y_max;
    Float z_max;
}
Voxel;

typedef
struct KDNode {
    enum Plane plane;
    Coord coord;
    
    Object3d ** objects;
    int objects_count;
    
    struct KDNode * l;
    struct KDNode * r;
}
KDNode;

typedef
struct {
    KDNode * root;
    Voxel bounding_box;
}
KDTree;
//

typedef
struct {    
    // Array of pointers to 3d objects of scene
    Object3d ** objects;
    int objects_count;
    int last_object_index;
    
    KDTree * kd_tree;
    
    // Array of pointers to light sources
    LightSource3d ** light_sources;
    int light_sources_count;
    int last_light_source_index;
    
    Color background_color;
    
    // Required to return value from interval [0..1]
    Float (*fog_density)(const Float distance,
                         const void * fog_parameters);    
    void * fog_parameters;
}
Scene;

typedef
struct {
    Point3d camera_position;
    
    Float al_x;
    Float sin_al_x;
    Float cos_al_x;
    
    Float al_y;
    Float sin_al_y;
    Float cos_al_y;
    
    Float al_z;
    Float sin_al_z;
    Float cos_al_z;
    
    Float proj_plane_dist;
}
Camera;

/***************************************************
 *                     Render                      *
 ***************************************************/

void
render_scene(const Scene * const scene,
             const Camera * const camera,
             Canvas * canvas,
             const int num_threads);

/***************************************************
 *                     Scene                       *
 ***************************************************/

Scene *
new_scene(const int objects_count,
          const int light_sources_count,
          const Color background_color);

void
release_scene(Scene * scene);

void
add_object(Scene * const scene,
           Object3d * const object);

void
prepare_scene(Scene * const scene);

void
set_exponential_fog(Scene * const scene,
                    const Float k);

void
set_no_fog(Scene * const scene);

Color
trace(const Scene * const scene,
      const Camera * const camera,
      Vector3d vector);

void
add_light_source(Scene * const scene,
                 LightSource3d * const light_source);

/***************************************************
 *                    3D objects                   *
 ***************************************************/

Object3d *
new_triangle(const Point3d p1,
             const Point3d p2,
             const Point3d p3,
             const Color color,
             const Material material);

Object3d *
new_triangle_with_norms(const Point3d p1,
                        const Point3d p2,
                        const Point3d p3,
                        const Vector3d n1,
                        const Vector3d n2,
                        const Vector3d n3,
                        const Color color,
                        const Material material);

Object3d *
new_triangle_with_texture(const Point3d p1,
                          const Point3d p2,
                          const Point3d p3,
                          const Point2d t1,
                          const Point2d t2,
                          const Point2d t3,
                          Canvas * texture,
                          const Color color,
                          const Material material);

Object3d *
new_sphere(const Point3d center,
           const Float radius,
           const Color color,
           const Material material);

void
release_object3d(Object3d * obj);

LightSource3d *
new_light_source(const Point3d location,
             const Color color);

Material
material(const Float Ka,
         const Float Kd,
         const Float Ks,
         const Float Kr,
         const Float Kt,
         const Float p);

/***************************************************
 *                     Camera                      *
 ***************************************************/

Camera *
new_camera(const Point3d camera_position,
           const Float al_x,
           const Float al_y,
           const Float al_z,
           const Float proj_plane_dist);

void
release_camera(Camera * const cam);

void
rotate_camera(Camera * const cam,
              const Float al_x,
              const Float al_y,
              const Float al_z);

void
move_camera(Camera * const camera,
            const Vector3d vector);

/***************************************************
 *                Point and vectors                *
 ***************************************************/

static inline Point2d
point2d(const Float x,
        const Float y);

static inline Point3d
point3d(const Float x,
        const Float y,
        const Float z);

static inline Vector3d
vector3dp(const Point3d start_point,
          const Point3d end_point);

static inline Vector3d
vector3df(const Float x,
          const Float y,
          const Float z);

static inline Point2d
point2d(const Float x,
        const Float y) {
    
	const Point2d p = {.x = x, .y = y};
	return p;
}

static inline Point3d
point3d(const Float x,
        const Float y,
        const Float z) {
    
	const Point3d p = {.x = x, .y = y, .z = z};
	return p;
}

static inline Vector3d
vector3dp(const Point3d start_point,
          const Point3d end_point) {
    
	const Vector3d v = {.x = (end_point.x - start_point.x),
                        .y = (end_point.y - start_point.y),
                        .z = (end_point.z - start_point.z)};
	return v;
}

static inline Vector3d
vector3df(const Float x,
          const Float y,
          const Float z) {
    
	const Vector3d v = {.x = x, .y = y, .z = z};
    return v;
}
#endif //__RENDER_H__
