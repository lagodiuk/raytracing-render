#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <render.h>
#include <utils.h>
#include <color.h>
#include <kdtree.h>

// Declarations
// --------------------------------------------------------------

static inline void rebuild_kd_tree(Scene * scene);

// Code
// --------------------------------------------------------------

Scene * new_scene(int objects_count, int light_sources_count, Color background_color) {
    Scene * s = malloc(sizeof(Scene));
    s->al = 0;
    s->be = 0;
    s->objects_count=objects_count;
    s->objects = calloc(objects_count, sizeof(Object3d *));
    if(light_sources_count) {
        s->light_sources = calloc(light_sources_count, sizeof(LightSource3d *));
    }
    s->light_sources_count = light_sources_count;
    s->background_color = background_color;
    s->last_object_index = -1;
    s->last_light_source_index = -1;
    s->fog_parameters = NULL;
    s->fog_density = NULL;
    
    s->kd_tree = build_kd_tree(s->objects, s->last_object_index + 1);
    return s;
}

void release_scene(Scene * scene) {
    int i;
    
    for(i = 0; i < scene->objects_count; i++) {
        if(scene->objects[i]) {
            release_object3d(scene->objects[i]);
        }
    }
    
    for(i = 0; i < scene->light_sources_count; i++) {
        if(scene->light_sources[i]) {
            free(scene->light_sources[i]);
        }
    }
    
    free(scene->objects);
    free(scene->light_sources);
    
    if(scene->fog_parameters) {
        free(scene->fog_parameters);
    }
    
    release_kd_tree(scene->kd_tree);    
    free(scene);
}

void rotate_scene(Scene * scene, Float al, Float be, Boolean rotate_light_sources) {
    scene->al = al;
    scene->be = be;
    
    // Pre-calculating of trigonometric functions
    Float sin_al = sin(al);
    Float cos_al = cos(al);
    Float sin_be = sin(be);
    Float cos_be = cos(be);
    
    int i;
    Object3d * obj;
    
    for(i = 0; i < scene->objects_count; i++) {
        if(scene->objects[i]) {
            obj = scene->objects[i];
            
            obj->rotate(obj->data, sin_al, cos_al, sin_be, cos_be);
        }
    }
    
    if((scene->light_sources_count) && (rotate_light_sources)) {
        Point3d ls_location;
        
        for(i = 0; i < scene->light_sources_count; i++) {
            if(scene->light_sources[i]) {
                ls_location = scene->light_sources[i]->location_world;
                
                scene->light_sources[i]->location =
                    rotate_point(ls_location, sin_al, cos_al, sin_be, cos_be);
            }
        }
    }
    
    rebuild_kd_tree(scene);
}

void add_object(Scene * scene, Object3d * object) {
    Float sin_al = sin(scene->al);
    Float cos_al = cos(scene->al);
    Float sin_be = sin(scene->be);
    Float cos_be = cos(scene->be);
    
    // Rotate object into current projection of scene
    object->rotate(object->data, sin_al, cos_al, sin_be, cos_be);
    
    scene->objects[++scene->last_object_index] = object;
    
    rebuild_kd_tree(scene);
}

static inline void rebuild_kd_tree(Scene * scene) {
    release_kd_tree(scene->kd_tree);
    scene->kd_tree = build_kd_tree(scene->objects, scene->last_object_index + 1);
}