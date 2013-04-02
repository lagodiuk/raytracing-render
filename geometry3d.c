#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <geometry3d.h>

#define EPSILON 1e-5

// General functions
inline Float cos_vectors3d(Vector3d v1, Vector3d v2);
inline Float module_vector3d(Vector3d v);
inline Float herons_square(Float a, Float b, Float c);
inline Point3d rotate(Point3d p, Float sin_al, Float cos_al, Float sin_be, Float cos_be);

/***************************************************
 *                Helpful functions                *
 ***************************************************/

inline void release_object3d(Object3d * obj) {
    obj->release_data(obj->data);
    free(obj);
}

inline Point3d point3d(Float x, Float y, Float z) {
	Point3d p = {.x = x, .y = y, .z = z};
	return p;
}

inline Point3d rotate(Point3d p, Float sin_al, Float cos_al, Float sin_be, Float cos_be) {
	Float x = p.x * cos_al - p.y * sin_al;
	Float y = p.x * sin_al * cos_be + p.y * cos_al * cos_be - p.z * sin_be;
	Float z = p.x * sin_al * sin_be + p.y * cos_al * sin_be + p.z * cos_be;
	return point3d(x, y, z);
}

inline Vector3d vector3dp(Point3d start_point, Point3d end_point) {
	Vector3d v = {.x = (end_point.x - start_point.x),
                  .y = (end_point.y - start_point.y),
                  .z = (end_point.z - start_point.z)};
	return v;
}

inline Vector3d vector3df(Float x, Float y, Float z) {
	Vector3d v = {.x = x, .y = y, .z = z};
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

void rotate_scene(Scene * scene, Float al, Float be) {
    // Pre-calculating of trigonometric functions
    Float sin_al = sin(al);
    Float cos_al = cos(al);
    Float sin_be = sin(be);
    Float cos_be = cos(be);
    
    int i;
    Object3d * obj;
    
    for(i = 0; i < scene->objects_count; i++) {
        obj = (scene->objects)[i];
        obj->rotate(obj->data, sin_al, cos_al, sin_be, cos_be);
    }
}

void trace(Scene * scene,
           Point3d vector_start,
           Vector3d vector,
           Color * color) {

    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    Float nearest_intersection_point_dist = FLT_MAX;

    int i;
    Object3d * obj = NULL;
    Point3d intersection_point;
    Float curr_intersection_point_dist;

    // Finding nearest object
    // and intersection point
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
                                        scene->light_sources_count);
        return;
    }
    
    *color = BACKGROUND_COLOR;
}

/***************************************************
 *                General functions                *
 ***************************************************/

inline Float module_vector3d(Vector3d v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Float cos_vectors3d(Vector3d v1, Vector3d v2) {
    Float numerator = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    Float denominator = module_vector3d(v1) * module_vector3d(v2);
    return numerator / denominator;
}

inline Float herons_square(Float a, Float b, Float c) {
    Float p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

/***************************************************
 *              Triangle3D construction            *
 ***************************************************/

void release_triangle_data(void * data);

void rotate_triangle(void * data,
                     Float sin_al,
                     Float cos_al,
                     Float sin_be,
                     Float cos_be);

Color get_triangle_color(void * data,
                         Point3d intersection_point,
                         LightSource3d * light_sources,
                         int light_sources_count);

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

void rotate_triangle(void * data, Float sin_al, Float cos_al, Float sin_be, Float cos_be) {
	Triangle3d * triangle = data;

	triangle->p1 = rotate(triangle->p1w, sin_al, cos_al, sin_be, cos_be);
	triangle->p2 = rotate(triangle->p2w, sin_al, cos_al, sin_be, cos_be);
	triangle->p3 = rotate(triangle->p3w, sin_al, cos_al, sin_be, cos_be);

	Point3d norm = rotate(point3d(triangle->Aw, triangle->Bw, triangle->Cw), sin_al, cos_al, sin_be, cos_be);
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
        return 0;
    }
    
	Float k = - (tr->A * vector_start.x
                 + tr->B * vector_start.y
                 + tr->C * vector_start.z
                 + tr->D)
              / scalar_product;
    
    if(k < EPSILON) {
        // Avoid intersection in the opposite direction
        return 0;
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
	Float d_p1_ipt = module_vector3d(vector3dp(tr->p1, ipt));
	Float d_p2_ipt = module_vector3d(vector3dp(tr->p2, ipt));
	Float d_p3_ipt = module_vector3d(vector3dp(tr->p3, ipt));
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
        return 1;
    }
    
    // No intersection
	return 0;
}

Color get_triangle_color(void * data,
                         Point3d intersection_point,
                         LightSource3d * light_sources,
                         int light_sources_count) {
	Triangle3d * triangle = data;
    // TODO
	/*
    int i;
	Float cosin;
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
