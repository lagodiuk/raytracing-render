#include <color.h>

#define BACKGROUND_COLOR rgb(0, 0, 0)


typedef
double
Float;

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
	Point3d location;
	Color color;
}
LightSource3d;

typedef 
struct {
	void * data;
        
	void (*rotate)(void * data,
                   Float al,
                   Float be);
    
	int (*intersect)(void * data,
                     Point3d vector_start,
                     Vector3d vector,
                     Point3d * intersection_point);
    
	Color (*get_color)(void * data,
                       Point3d intersection_point,
                       LightSource3d * light_sources,
                       int light_sources_count);
    
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
}
Scene;

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
}
Triangle3d;

/***************************************************
 *                Helpful functions                *
 ***************************************************/

inline void release_object3d(Object3d * obj);

inline Point3d point3d(Float x, Float y, Float z);

inline Point3d rotate(Point3d p, Float al, Float be);

inline Vector3d vector3dp(Point3d start_point, Point3d end_point);

inline Vector3d vector3df(Float x, Float y, Float z);

inline LightSource3d light_source_3d(Point3d location, Color color);

/***************************************************
 *                     Scene                       *
 ***************************************************/

inline Scene * new_scene(int objects_count);

inline void release_scene(Scene * scene);

void rotate_scene(Scene * scene, Float al, Float be);

void trace(Scene * scene,
          Point3d vector_start,
          Vector3d vector,
          Color * color);

/***************************************************
 *              3D objects construction            *
 ***************************************************/

Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3, Color color);
