#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <geometry3d.h>

// Triangle3d functions
void release_triangle_data(void * data);
void print_triangle(void * data);
void rotate_triangle(void * data, float al, float be);
Color get_triangle_color(void * data, Point3d p, LightSource3d * light_sources, int light_sources_count);
Point3d intersect_triangle(void * data, Point3d p, Vector3d v);

inline Point3d point3d(float x, float y, float z) {
	Point3d p = {.x = x, .y = y, .z = z};
	return p;
}


inline LightSource3d light_source_3d(Point3d location, Color color) {
	LightSource3d l = {.location = location, .color = color};
	return l;
}

inline Vector3d vector3dp(Point3d start_point, Point3d end_point) {
	Point3d direction = point3d(
		end_point.x - start_point.x,
		end_point.y - start_point.y,
		end_point.z - start_point.z);
	Vector3d v = {.direction = direction};
	return v;
}

inline Vector3d vector3df(float x, float y, float z) {
	Point3d direction = point3d(x, y, z);
	Vector3d v = {.direction = direction};
        return v;
}

Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3) {
	Triangle3d * triangle = malloc(sizeof(Triangle3d));
	triangle->p1w = p1;
	triangle->p2w = p2;
	triangle->p3w = p3;
	triangle->Aw = (p1.y - p3.y) * (p2.z - p3.z) - (p1.z - p3.z) * (p2.y - p3.y);
	triangle->Bw = (p2.x - p3.x) * (p1.z - p3.z) - (p2.z - p3.z) * (p1.x - p3.x);
	triangle->Cw = (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
	triangle->Dw = -(p1.x * triangle->Aw + p1.y * triangle->Bw + p1.z * triangle->Cw);

	Object3d * obj = malloc(sizeof(Object3d));
	obj->data = triangle;
	obj->print = print_triangle;
	obj->rotate = rotate_triangle;
	obj->release_data = release_triangle_data;
	obj->get_color = get_triangle_color;
	obj->intersect = intersect_triangle;

	return obj;
}

void print_triangle(void * data) {
	Triangle3d * t = data;

	printf("x1w = %f y1w = %f z1w = %f\n", t->p1w.x, t->p1w.y, t->p1w.z);
	printf("x2w = %f y2w = %f z2w = %f\n", t->p2w.x, t->p2w.y, t->p2w.z);
	printf("x3w = %f y3w = %f z3w = %f\n", t->p3w.x, t->p3w.y, t->p3w.z);
	printf("Aw = %f Bw = %f Cw = %f Dw = %f\n", t->Aw, t->Bw, t->Cw, t->Dw);

	putchar('\n');	
	printf("x1 = %f y1 = %f z1 = %f\n", t->p1.x, t->p1.y, t->p1.z);
	printf("x2 = %f y2 = %f z2 = %f\n", t->p2.x, t->p2.y, t->p2.z);
	printf("x3 = %f y3 = %f z3 = %f\n", t->p3.x, t->p3.y, t->p3.z);
	printf("A = %f B = %f C = %f D = %f\n", t->A, t->B, t->C, t->D);
}

void rotate_triangle(void * data, float al, float be) {
	Triangle3d * triangle = data;

	triangle->p1 = rotate(triangle->p1w, al, be);
	triangle->p2 = rotate(triangle->p2w, al, be);
	triangle->p3 = rotate(triangle->p3w, al, be);

	Point3d norm = rotate(point3d(triangle->Aw, triangle->Bw, triangle->Cw), al, be);
	triangle->A = norm.x;
	triangle->B = norm.y;
	triangle->C = norm.z;
	triangle->D = -(triangle->p1.x * triangle->A + triangle->p1.y * triangle->B + triangle->p1.z * triangle->C);
}

Point3d intersect_triangle(void * data, Point3d p, Vector3d v) {
	Triangle3d * tr = data;

	float k = - (tr->A * p.x + tr->B * p.y + tr->C * p.z + tr->D)
		/ (tr->A * v.direction.x + tr->B * v.direction.y + tr->C * v.direction.z);
	
	float x = p.x + v.direction.x * k;
	float y = p.y + v.direction.y * k;
	float z = p.z + v.direction.z * k;

	return point3d(x, y, z);
}

Color get_triangle_color(void * data, Point3d p, LightSource3d * light_sources, int light_sources_count) {
	Triangle3d * triangle = data;
	int i;
	float cosin;
	Color c = BACKGROUND_COLOR;
	Vector3d norm = vector3df(triangle->A, triangle->B, triangle->C);
	for(i = 0; i < light_sources_count; i++) {
		Vector3d light_vect = vector3dp(p, light_sources[i].location);
		cosin = cos_vectors3d(norm, light_vect);
		c = add_colors(c, mul_color(light_sources[i].color, cosin));
	}
	return c;
}

void release_triangle_data(void * data) {
	Triangle3d * triangle = data;
	free(triangle);
}

inline Vector3d vector_product(Vector3d u, Vector3d v) {
	float x = u.direction.y * v.direction.z - u.direction.z * v.direction.y;
	float y = u.direction.z * v.direction.x - u.direction.x * v.direction.z;
	float z = u.direction.x * v.direction.y - u.direction.y * v.direction.x;
	return vector3df(x, y, z);
}

inline Point3d rotate(Point3d p, float al, float be) {
	float x = p.x * cos(al) - p.y * sin(al);
	float y = p.x * sin(al) * cos(be) + p.y * cos(al) * cos(be) - p.z * sin(be);
	float z = p.x * sin(al) * sin(be) + p.y * cos(al) * sin(be) + p.z * cos(be);
	return point3d(x, y, z);
}

inline float module_vector3d(Vector3d v) {
        return v.direction.x * v.direction.x
                + v.direction.y * v.direction.y
                + v.direction.z * v.direction.z;
}

inline float cos_vectors3d(Vector3d v1, Vector3d v2) {
        float numerator = v1.direction.x * v2.direction.x
                + v1.direction.y * v2.direction.y
                + v1.direction.z * v2.direction.z;
        float denominator = module_vector3d(v1) * module_vector3d(v2);
        return numerator / denominator;
}
