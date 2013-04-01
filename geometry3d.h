#include <color.h>

#define BACKGROUND_COLOR rgb(0, 0, 0)

typedef 
struct {
	float x;
	float y;
	float z;
}
Point3d;

typedef
struct {
	Point3d direction;
}
Vector3d;

typedef
struct {
	Point3d location;
	Color color;
}
LightSource3d;

typedef 
struct {
	void * data;
        
	void (*rotate)(void * data,
                   float al,
                   float be);
    
	int (*intersect)(void * data,
                     Point3d vector_start,
                     Vector3d vector,
                     Point3d * intersection_point);
    
	Color (*get_color)(void * data,
                       Point3d intersection_point,
                       LightSource3d * light_sources,
                       int light_sources_count,
                       Point3d vector_start,
                       Vector3d vector);
    
	void (*release_data)(void * data);
}
Object3d;

typedef
struct {
    Object3d ** objects;
    int objects_count;
    
    LightSource3d * light_sources;
    int light_sources_count;
}
Scene;

typedef
struct {
    /************
     * Geometry *
     ************/
    
    // World coordinates
	Point3d p1w;
	Point3d p2w;
	Point3d p3w;
    // World norm vector (Aw, Bw, Cw)
	float Aw;
	float Bw;
	float Cw;
	float Dw;
    // Projection coordinates
	Point3d p1;
	Point3d p2;
	Point3d p3;
    // Projection norm vector (A, B, C)
	float A;
	float B;
	float C;
	float D;
    // Length of the sides of a triangle
    float d_p1_p2;
    float d_p2_p3;
    float d_p3_p1;
    // Square of triangle
    float s;

    /************
     * Material *
     ************/
    
    Color color;
}
Triangle3d;

/***************************************************
 *                Helpful functions                *
 ***************************************************/

inline void release_object3d(Object3d * obj);

inline Point3d point3d(float x, float y, float z);

inline Point3d rotate(Point3d p, float al, float be);

inline Vector3d vector3dp(Point3d start_point, Point3d end_point);

inline Vector3d vector3df(float x, float y, float z);

inline LightSource3d light_source_3d(Point3d location, Color color);

/***************************************************
 *                     Scene                       *
 ***************************************************/

inline Scene * new_scene(int objects_count);

inline void release_scene(Scene * scene);

void rotate_scene(Scene * scene, float al, float be);

void trace(Scene * scene,
          Point3d vector_start,
          Vector3d vector,
          Color * color);

/***************************************************
 *              3D objects construction            *
 ***************************************************/

Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3, Color color);
