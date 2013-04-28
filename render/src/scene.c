#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include <render.h>
#include <utils.h>
#include <color.h>
#include <kdtree.h>

// Declarations
// --------------------------------------------------------------

static inline void
rebuild_kd_tree(Scene * scene);

// Code
// --------------------------------------------------------------

Scene *
new_scene(const int objects_count,
          const int light_sources_count,
          const Color background_color) {
    
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
    
    s->kd_tree = NULL;
    return s;
}

void
release_scene(Scene * scene) {
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
    
    if(scene->kd_tree)
        release_kd_tree(scene->kd_tree);
    free(scene);
}

void
rotate_scene(Scene * const scene,
             const Float be,
             const Float al,
             const Boolean rotate_light_sources) {
    
    if(fabs(scene->be - be) > EPSILON) {
        scene->be = be;
        scene->sin_be = sin(scene->be);
        scene->cos_be = cos(scene->be);
    }

    if(fabs(scene->al - al) > EPSILON) {
        scene->al = al;
        scene->sin_al = sin(scene->al);
        scene->cos_al = cos(scene->al);
    }
    
    int i;
    LightSource3d * light;
    for(i = 0; i < scene->light_sources_count; i++) {
        if(scene->light_sources[i]) {
            light = scene->light_sources[i];
            
            if(!rotate_light_sources) {
                light->location = rotate_point(light->location_world,
                                               scene->sin_al,
                                               scene->cos_al,
                                               scene->sin_be,
                                               scene->cos_be);
            } else {
                light->location = light->location_world;
            }
        }
    }
}

void
add_object_and_prepare_scene(Scene * const scene,
                             Object3d * const object) {
        
    scene->objects[++scene->last_object_index] = object;
    
    rebuild_kd_tree(scene);
}

void
add_object(Scene * const scene,
           Object3d * const object) {
    
    scene->objects[++scene->last_object_index] = object;
}

void
prepare_scene(Scene * const scene) {
    rebuild_kd_tree(scene);    
}

void
add_light_source(Scene * const scene,
                 LightSource3d * const light_source) {
    
    scene->light_sources[++scene->last_light_source_index] = light_source;
}

static inline void
rebuild_kd_tree(Scene * scene) {
    if(scene->kd_tree)
        release_kd_tree(scene->kd_tree);
    scene->kd_tree = build_kd_tree(scene->objects, scene->last_object_index + 1);
}