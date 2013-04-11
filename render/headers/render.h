#ifndef __RENDEER_H__
#define __RENDEER_H__

#include <float.h>
#include <color.h>

typedef
int
Boolean;

#define True 1
#define False 0

// Using double for satisfactory accuracy
typedef
double
Float;

#define FLOAT_MAX DBL_MAX

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
        
	void (*rotate)(void * data,
                   Float sin_al,
                   Float cos_al,
                   Float sin_be,
                   Float cos_be);
    
	Boolean (*intersect)(void * data,
                         Point3d vector_start,
                         Vector3d vector,
                         Point3d * intersection_point);
    
	Color (*get_color)(void * data,
                       Point3d intersection_point);
    
    Vector3d (*get_normal_vector)(void * data,
                                  Point3d intersection_point);
    
    Material (*get_material)(void * data,
                             Point3d intersection_point);
    
    Point3d (*get_min_boundary_point)(void * data);
    
    Point3d (*get_max_boundary_point)(void * data);
    
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
    // Angles of projection
    Float al;
    Float be;
    
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
    Float (*fog_density)(Float distance,
                         void * fog_parameters);    
    void * fog_parameters;
}
Scene;

/***************************************************
 *                     Scene                       *
 ***************************************************/

Scene *new_scene(int objects_count,
                 int light_sources_count,
                 Color background_color);

void release_scene(Scene * scene);

void rotate_scene(Scene * scene,
                  Float al,
                  Float be,
                  Boolean rotate_light_sources);

void add_object(Scene * scene,
                Object3d * object);

void set_exponential_fog(Scene * scene,
                         Float k);

void set_no_fog(Scene * scene);

void trace(Scene * scene,
           Point3d vector_start,
           Vector3d vector,
           Color * color);

static inline void add_light_source(Scene * scene,
                                    LightSource3d * light_source) {    
    scene->light_sources[++scene->last_light_source_index] = light_source;
}

/***************************************************
 *                    3D objects                   *
 ***************************************************/

Object3d * new_triangle(Point3d p1,
                        Point3d p2,
                        Point3d p3,
                        Color color,
                        Material material);

/***************************************************
 *                Helpful functions                *
 ***************************************************/

static inline void release_object3d(Object3d * obj);

static inline Point3d point3d(Float x,
                              Float y,
                              Float z);

static inline Vector3d vector3dp(Point3d start_point,
                                 Point3d end_point);

static inline Vector3d vector3df(Float x,
                                 Float y,
                                 Float z);

static inline LightSource3d * light_source_3d(Point3d location,
                                              Color color);

static inline Material material(Float Ka,
                                Float Kd,
                                Float Ks,
                                Float Kr,
                                Float Kt,
                                Float p);

static inline void release_object3d(Object3d * obj) {
    obj->release_data(obj->data);
    free(obj);
}

static inline Point3d point3d(Float x,
                              Float y,
                              Float z) {
    
	Point3d p = {.x = x, .y = y, .z = z};
	return p;
}

static inline Vector3d vector3dp(Point3d start_point,
                                 Point3d end_point) {
    
	Vector3d v = {.x = (end_point.x - start_point.x),
                  .y = (end_point.y - start_point.y),
                  .z = (end_point.z - start_point.z)};
	return v;
}

static inline Vector3d vector3df(Float x,
                                 Float y,
                                 Float z) {
    
	Vector3d v = {.x = x, .y = y, .z = z};
    return v;
}

static inline LightSource3d * light_source_3d(Point3d location,
                                              Color color) {
    
	LightSource3d * ls_p = malloc(sizeof(LightSource3d));
    
    ls_p->location_world = location;
    ls_p->location = location;
    ls_p->color = color;
    
	return ls_p;
}

static inline Material material(Float Ka,
                                Float Kd,
                                Float Ks,
                                Float Kr,
                                Float Kt,
                                Float p) {
    
    Float sum = Ka + Kd + Ks + Kr + Kt;
    Material m = {.Ka = Ka / sum,
                  .Kd = Kd / sum,
                  .Ks = Ks / sum,
                  .Kr = Kr / sum,
                  .Kt = Kt / sum,
                  .p = p};
    return m;
}
#endif
