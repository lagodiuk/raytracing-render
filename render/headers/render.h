#ifndef GEOMETRY_3D_H
#define GEOMETRY_3D_H

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
    
    // Required:
    // Ka + Kd + Ks + Kr + Kt = 1.0
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
    
	int (*intersect)(void * data,
                     Point3d vector_start,
                     Vector3d vector,
                     Point3d * intersection_point);
    
	Color (*get_color)(void * data,
                       Point3d intersection_point,
                       LightSource3d * light_sources,
                       int light_sources_count);
    
    Vector3d (*get_normal_vector)(void * data,
                                  Point3d intersection_point);
    
    Material (*get_material)(void * data,
                             Point3d intersection_point);
    
	void (*release_data)(void * data);
}
Object3d;

typedef
struct {
    // Array of pointers to 3d objects of scene
    Object3d ** objects;
    int objects_count;
    
    // Array of light sources
    LightSource3d * light_sources;
    int light_sources_count;
    
    Color background_color;
}
Scene;

/***************************************************
 *                Helpful functions                *
 ***************************************************/

inline void release_object3d(Object3d * obj);

inline Point3d point3d(Float x, Float y, Float z);

inline Vector3d vector3dp(Point3d start_point, Point3d end_point);

inline Vector3d vector3df(Float x, Float y, Float z);

inline LightSource3d light_source_3d(Point3d location, Color color);

inline Material material(Float Ka, Float Kd, Float Ks, Float Kr, Float Kt);

/***************************************************
 *                     Scene                       *
 ***************************************************/

inline Scene * new_scene(int objects_count, int light_sources_count, Color background_color);

inline void release_scene(Scene * scene);

void rotate_scene(Scene * scene, Float al, Float be, Boolean rotate_light_sources);

void trace(Scene * scene,
          Point3d vector_start,
          Vector3d vector,
          Color * color);

/***************************************************
 *              3D objects construction            *
 ***************************************************/

Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3, Color color, Material material);

#endif