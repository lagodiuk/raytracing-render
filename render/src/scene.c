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

void
release_object3d(Object3d * obj) {
    obj->release_data(obj->data);
    free(obj);
}

LightSource3d *
light_source_3d(const Point3d location,
                const Color color) {
    
	LightSource3d * ls_p = malloc(sizeof(LightSource3d));
    
    ls_p->location_world = location;
    ls_p->location = location;
    ls_p->color = color;
    
	return ls_p;
}

Material
material(const Float Ka,
         const Float Kd,
         const Float Ks,
         const Float Kr,
         const Float Kt,
         const Float p) {
    
    Float sum = Ka + Kd + Ks + Kr + Kt;
    Material m = {.Ka = Ka / sum,
        .Kd = Kd / sum,
        .Ks = Ks / sum,
        .Kr = Kr / sum,
        .Kt = Kt / sum,
        .p = p};
    return m;
}

Camera *
new_camera(const Point3d camera_position,
           const Float be,
           const Float al,
           const Float proj_plane_dist) {
    
    
    Camera * cam = malloc(sizeof(Camera));
    
    cam->camera_position = camera_position;
    
    cam->al = al;
    cam->sin_al = sin(al);
    cam->cos_al = cos(al);
    
    cam->be = be;
    cam->sin_be = sin(be);
    cam->cos_be = cos(be);
    
    cam->proj_plane_dist = proj_plane_dist;
    
    return cam;
}

void
delete_camera(Camera * const cam) {
    free(cam);
}

void
rotate_camera(Camera * const cam,
              const Float be,
              const Float al) {
    
    if(fabs(be) > EPSILON) {
        cam->be += be;
        cam->sin_be = sin(cam->be);
        cam->cos_be = cos(cam->be);
    }
    
    if(fabs(al) > EPSILON) {
        cam->al += al;
        cam->sin_al = sin(cam->al);
        cam->cos_al = cos(cam->al);
    }
}

void
move_camera(Camera * const camera,
            const Vector3d vector) {
    
    Vector3d r_vector = rotate_vector(vector, camera->sin_al, camera->cos_al, camera->sin_be, camera->cos_be);
    
    Point3d curr_pos = camera->camera_position;
    
    camera->camera_position = point3d(curr_pos.x + r_vector.x,
                                      curr_pos.y + r_vector.y,
                                      curr_pos.z + r_vector.z);    
}