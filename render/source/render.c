#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <render.h>
#include <utils.h>

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

inline Scene * new_scene(int objects_count, Color background_color) {
    Scene * s = malloc(sizeof(Scene));
    s->objects_count=objects_count;
    s->objects = calloc(objects_count, sizeof(Object3d *));
    s->background_color = background_color;
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
    
    *color = scene->background_color;
}