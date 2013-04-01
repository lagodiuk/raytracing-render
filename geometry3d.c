#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <geometry3d.h>

#define EPSILON 1e-5

// General functions
inline float cos_vectors3d(Vector3d v1, Vector3d v2);
inline float module_vector3d(Vector3d v);
inline float herons_square(float a, float b, float c);

/***************************************************
 *                Helpful functions                *
 ***************************************************/

inline void release_object3d(Object3d * obj) {
    obj->release_data(obj->data);
    free(obj);
}

inline Point3d point3d(float x, float y, float z) {
	Point3d p = {.x = x, .y = y, .z = z};
	return p;
}

inline Point3d rotate(Point3d p, float al, float be) {
	float x = p.x * cos(al) - p.y * sin(al);
	float y = p.x * sin(al) * cos(be) + p.y * cos(al) * cos(be) - p.z * sin(be);
	float z = p.x * sin(al) * sin(be) + p.y * cos(al) * sin(be) + p.z * cos(be);
	return point3d(x, y, z);
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

inline LightSource3d light_source_3d(Point3d location, Color color) {
	LightSource3d l = {.location = location, .color = color};
	return l;
}

/***************************************************
 *                     Scene                       *
 ***************************************************/

inline Scene * new_scene(int objects_count) {
    Scene * s = malloc(sizeof(Scene));
    s->objects_count=objects_count;
    s->objects = calloc(objects_count, sizeof(Object3d *));
    return s;
}

inline void release_scene(Scene * scene) {
    int i;
    for(i = 0; i < scene->objects_count; i++) {
        if(scene->objects[i]) {
            release_object3d((scene->objects)[i]);
        }
    }
    free(scene->objects);
    free(scene);
}

void rotate_scene(Scene * scene, float al, float be) {
    int i;
    Object3d * obj;
    for(i = 0; i < scene->objects_count; i++) {
        obj = (scene->objects)[i];
        obj->rotate(obj->data, al, be);
    }
}

void trace(Scene * scene,
           Point3d vector_start,
           Vector3d vector,
           Color * color) {

    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    float nearest_intersection_point_dist = FLT_MAX;

    int i;
    Object3d * obj = NULL;
    Point3d intersection_point;
    float curr_intersection_point_dist;

    for(i = 0; i < scene->objects_count; i++) {
        obj = (scene->objects)[i];
        
        if(obj->intersect(obj->data, vector_start, vector, &intersection_point)) {
            curr_intersection_point_dist = module_vector3d(vector3dp(vector_start, intersection_point));
            
            if(curr_intersection_point_dist < nearest_intersection_point_dist) {
                nearest_obj = obj;
                nearest_intersection_point = intersection_point;
                nearest_intersection_point_dist = curr_intersection_point_dist;
            }
        }
    }

    if(nearest_obj) {
        *color = nearest_obj->get_color(nearest_obj->data,
                                        nearest_intersection_point,
                                        scene->light_sources,
                                        scene->light_sources_count,
                                        vector_start,
                                        vector);
        return;
    }
    
    *color = BACKGROUND_COLOR;
}

/***************************************************
 *                General functions                *
 ***************************************************/

inline float module_vector3d(Vector3d v) {
    return sqrt(v.direction.x * v.direction.x
                + v.direction.y * v.direction.y
                + v.direction.z * v.direction.z);
}

inline float cos_vectors3d(Vector3d v1, Vector3d v2) {
    float numerator = v1.direction.x * v2.direction.x
    + v1.direction.y * v2.direction.y
    + v1.direction.z * v2.direction.z;
    float denominator = module_vector3d(v1) * module_vector3d(v2);
    return numerator / denominator;
}

inline float herons_square(float a, float b, float c) {
    float p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

/***************************************************
 *              Triangle3D construction            *
 ***************************************************/

void release_triangle_data(void * data);

void rotate_triangle(void * data,
                     float al,
                     float be);

Color get_triangle_color(void * data,
                         Point3d intersection_point,
                         LightSource3d * light_sources,
                         int light_sources_count,
                         Point3d vector_start,
                         Vector3d vector);

int intersect_triangle(void * data,
                       Point3d vector_start,
                       Vector3d vector,
                       Point3d * intersection_point);


Object3d * new_triangle(Point3d p1, Point3d p2, Point3d p3, Color color) {
	Triangle3d * triangle = malloc(sizeof(Triangle3d));
	triangle->p1w = p1;
	triangle->p2w = p2;
	triangle->p3w = p3;
	triangle->Aw = (p1.y - p3.y) * (p2.z - p3.z) - (p1.z - p3.z) * (p2.y - p3.y);
	triangle->Bw = (p2.x - p3.x) * (p1.z - p3.z) - (p2.z - p3.z) * (p1.x - p3.x);
	triangle->Cw = (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
	triangle->Dw = -(p1.x * triangle->Aw + p1.y * triangle->Bw + p1.z * triangle->Cw);
    triangle->d_p1_p2 = module_vector3d(vector3dp(p1, p2));
    triangle->d_p2_p3 = module_vector3d(vector3dp(p2, p3));
    triangle->d_p3_p1 = module_vector3d(vector3dp(p3, p1));
    triangle->s = herons_square(triangle->d_p1_p2, triangle->d_p2_p3, triangle->d_p3_p1);
    triangle->color = color;

	Object3d * obj = malloc(sizeof(Object3d));
	obj->data = triangle;
	obj->rotate = rotate_triangle;
	obj->release_data = release_triangle_data;
	obj->get_color = get_triangle_color;
	obj->intersect = intersect_triangle;

	return obj;
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

int intersect_triangle(void * data, Point3d vector_start, Vector3d vector, Point3d * intersection_point) {
	Triangle3d * tr = data;

	float k = - (tr->A * vector_start.x + tr->B * vector_start.y + tr->C * vector_start.z + tr->D)
		/ (tr->A * vector.direction.x + tr->B * vector.direction.y + tr->C * vector.direction.z);
    
    if(k < EPSILON) {
        // No intersection
        return 0;
    }
	
	float x = vector_start.x + vector.direction.x * k;
	float y = vector_start.y + vector.direction.y * k;
	float z = vector_start.z + vector.direction.z * k;

    // Intersection point
	Point3d ipt = point3d(x, y, z);

	float d_p1_ipt = module_vector3d(vector3dp(tr->p1, ipt));
	float d_p2_ipt = module_vector3d(vector3dp(tr->p2, ipt));
	float d_p3_ipt = module_vector3d(vector3dp(tr->p3, ipt));
    
    float s1 = herons_square(tr->d_p1_p2, d_p1_ipt, d_p2_ipt);
    float s2 = herons_square(tr->d_p2_p3, d_p2_ipt, d_p3_ipt);
    float s3 = herons_square(tr->d_p3_p1, d_p3_ipt, d_p1_ipt);
    
    if(abs(s1 + s2 + s3 - tr->s) < EPSILON) {
        // Intersected
        *intersection_point = ipt;
        return 1;
    }
    
    // No intersection
	return 0;
}

Color get_triangle_color(void * data,
                         Point3d intersection_point,
                         LightSource3d * light_sources,
                         int light_sources_count,
                         Point3d vector_start,
                         Vector3d vector) {
	Triangle3d * triangle = data;
    // TODO
	/*
    int i;
	float cosin;
	Color c = BACKGROUND_COLOR;
	Vector3d norm = vector3df(triangle->A, triangle->B, triangle->C);
	for(i = 0; i < light_sources_count; i++) {
		Vector3d light_vect = vector3dp(intersection_point, light_sources[i].location);
		cosin = cos_vectors3d(norm, light_vect);
		c = add_colors(c, mul_color(light_sources[i].color, cosin));
	}
     */
	return triangle->color;
}

void release_triangle_data(void * data) {
	Triangle3d * triangle = data;
	free(triangle);
}
