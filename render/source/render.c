#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <render.h>
#include <utils.h>

int is_viewable(Point3d target_point, Point3d starting_point, Scene * scene);

Color get_lighting_color(Point3d point, Vector3d norm_v, Scene * scene);

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
	LightSource3d l = {.location_world = location,
                       .location = location,
                       .color = color};
	return l;
}

inline Material material(Float Ka, Float Kd, Float Ks, Float Kr, Float Kt) {
    Float sum = Ka + Kd + Ks + Kr + Kt;
    Material m = {.Ka = Ka / sum,
                  .Kd = Kd / sum,
                  .Ks = Ks / sum,
                  .Kr = Kr / sum,
                  .Kt = Kt / sum};
    return m;
}

/***************************************************
 *                     Scene                       *
 ***************************************************/

inline Scene * new_scene(int objects_count, int light_sources_count, Color background_color) {
    Scene * s = malloc(sizeof(Scene));
    s->objects_count=objects_count;
    s->objects = calloc(objects_count, sizeof(Object3d *));
    if(light_sources_count) {
        s->light_sources = calloc(light_sources_count, sizeof(LightSource3d));
    }
    s->light_sources_count = light_sources_count;
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

void rotate_scene(Scene * scene, Float al, Float be, Boolean rotate_light_sources) {
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
    
    if((scene->light_sources_count) && (rotate_light_sources)) {
        Point3d ls_location;
        
        for(i = 0; i < scene->light_sources_count; i++) {
            ls_location = scene->light_sources[i].location_world;
            scene->light_sources[i].location =
                rotate_point(ls_location, sin_al, cos_al, sin_be, cos_be);
        }
    }
}

void trace(Scene * scene,
           Point3d vector_start,
           Vector3d vector,
           Color * color) {

    normalize_vector(&vector);
    
    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    Float nearest_intersection_point_dist = FLOAT_MAX;

    int i;
    Object3d * obj = NULL;
    Point3d intersection_point;
    Float curr_intersection_point_dist;

    // Finding nearest object
    // and intersection point
    for(i = 0; i < scene->objects_count; i++) {
        obj = (scene->objects)[i];
        
        if(obj->intersect(obj->data, vector_start, vector, &intersection_point)) {
            curr_intersection_point_dist = module_vector(vector3dp(vector_start, intersection_point));
            
            if(curr_intersection_point_dist < nearest_intersection_point_dist) {
                nearest_obj = obj;
                nearest_intersection_point = intersection_point;
                nearest_intersection_point_dist = curr_intersection_point_dist;
            }
        }
    }

    if(nearest_obj) {
        Color obj_color = nearest_obj->get_color(nearest_obj->data,
                                        nearest_intersection_point,
                                        scene->light_sources,
                                        scene->light_sources_count);
        
        Vector3d norm = nearest_obj->get_normal_vector(nearest_obj->data, nearest_intersection_point);
        
        if(scene->light_sources_count) {
            Color light_color = get_lighting_color(nearest_intersection_point, norm, scene);
            obj_color = mul_colors(obj_color, light_color);
        }
        
        *color = obj_color;
        return;
    }
    
    *color = scene->background_color;
}

Color get_lighting_color(Point3d point, Vector3d norm_v, Scene * scene) {
    Color light_color = rgb(0, 0, 0);
    
    normalize_vector(&norm_v);
    
    LightSource3d ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    int i;
    
    for(i = 0; i < scene->light_sources_count; i++) {
        ls = scene->light_sources[i];
        
        // If not shaded
        if(is_viewable(ls.location, point, scene)) {
            v_ls = vector3dp(point, ls.location);
            normalize_vector(&v_ls);
            
            cos_ls = fabs(cos_vectors(norm_v, v_ls));
            color_ls = mul_color(ls.color, cos_ls);
            light_color = add_colors(light_color, color_ls);
        }
    }
    
    return light_color;
}

int is_viewable(Point3d target_point, Point3d starting_point, Scene * scene) {
    Vector3d ray = vector3dp(starting_point, target_point);
    normalize_vector(&ray);
    Point3d intersection_point;
    
    int i;
    Object3d * obj = NULL;
    for(i = 0; i < scene->objects_count; i++) {
        obj = (scene->objects)[i];
        
        if(obj->intersect(obj->data, starting_point, ray, &intersection_point)) {
            // Target point is not viewable from starting point
            // because of ray intersects some of scene objects
            return 0;
        }
    }
    
    // Ray doesn't intersect any of scene objects
    return 1;
}