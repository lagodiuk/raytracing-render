#include <float.h>
#include <color.h>

#define BACKGROUND_COLOR rgb(0, 0, 0)
#define NO_INTERSECTION point3d(FLT_MAX, FLT_MAX, FLT_MAX)

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
	void (*print)(void * data);
	void (*rotate)(void * data, float al, float be);
	Point3d (*intersect)(void * data, Point3d p, Vector3d v);
	Color (*get_color)(void * data, Point3d p, LightSource3d * light_sources, int light_sources_count);
	void (*release_data)(void * data);
}
Object3d;

typedef
struct {
	Point3d p1w;
	Point3d p2w;
	Point3d p3w;
	float Aw;
	float Bw;
	float Cw;
	float Dw;
	Point3d p1;
	Point3d p2;
	Point3d p3;
	float A;
	float B;
	float C;
	float D;
}
Triangle3d;

inline Point3d point3d(float x, float y, float z);

inline Point3d rotate(Point3d p, float al, float be);

inline Vector3d vector3dp(Point3d start_point, Point3d end_point);

inline Vector3d vector3df(float x, float y, float z);

inline Vector3d vector_product(Vector3d u, Vector3d v);

inline LightSource3d light_source_3d(Point3d location, Color color);

Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3);

inline float cos_vectors3d(Vector3d v1, Vector3d v2);

inline float module_vector3d(Vector3d v);
