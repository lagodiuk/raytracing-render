#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include <render.h>
#include <utils.h>
#include <kdtree.h>
#include <color.h>

#define INITIAL_RAY_INTENSITY 100
#define THRESHOLD_RAY_INTENSITY 10
#define MAX_RAY_RECURSION_LEVEL 10

// Declarations
// --------------------------------------------------------------

static inline Boolean
is_viewable(Point3d target_point,
            Point3d starting_point,
            Scene * scene);

static inline Color
get_lighting_color(Point3d point,
                   Vector3d norm_v,
                   Scene * scene);

static inline Color
get_specular_color(Point3d point,
                   Vector3d reflected_ray,
                   Scene * scene,
                   Float p);

static inline Color
calculate_color(Scene * scene,
                Point3d vector_start,
                Vector3d vector,
                Object3d ** obj_ptr,
                Point3d * point_ptr,
                Float * dist_ptr,
                Float intensity,
                int recursion_level);

void
trace_recursively(Scene * scene,
                  Point3d vector_start,
                  Vector3d vector,
                  Color * color,
                  Float intensity,
                  int recursion_level);

// Code
// --------------------------------------------------------------

void
trace(Scene * scene,
      Camera * camera,
      Vector3d vector,
      Color * color) {
    
    Vector3d r_vector = rotate_vector(vector, camera->sin_al, camera->cos_al, camera->sin_be, camera->cos_be);
    
    trace_recursively(scene,
                      camera->camera_position,
                      r_vector,
                      color,
                      INITIAL_RAY_INTENSITY,
                      0);
}

void
trace_recursively(Scene * scene,
                  Point3d vector_start,
                  Vector3d vector,
                  Color * color,
                  Float intensity,
                  int recursion_level) {

    normalize_vector(&vector);
    
    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    Float nearest_intersection_point_dist = FLOAT_MAX;
    
    if(find_intersection_tree(scene->kd_tree,
                              vector_start,
                              vector,
                              &nearest_obj,
                              &nearest_intersection_point,
                              &nearest_intersection_point_dist)) {

        *color = calculate_color(scene,
                                 vector_start,
                                 vector,
                                 &nearest_obj,
                                 &nearest_intersection_point,
                                 &nearest_intersection_point_dist,
                                 intensity,
                                 recursion_level);
                
        return;
    }
    
    *color = scene->background_color;
}

static inline Color
calculate_color(Scene * scene,
                Point3d vector_start,
                Vector3d vector,
                Object3d ** obj_ptr,
                Point3d * point_ptr,
                Float * dist_ptr,
                Float intensity,
                int recursion_level) {

    Object3d * obj = *obj_ptr;
    Point3d point = *point_ptr;
    Float dist = *dist_ptr;
    
    
    Material material = obj->get_material(obj->data, point);
    
    Vector3d norm = obj->get_normal_vector(obj->data, point);
    
    Color obj_color = obj->get_color(obj->data, point);
    Color ambient_color;
    Color diffuse_color;
    Color reflected_color;
    Color specular_color;
    
    Float fog_density = 0;
    if(scene->fog_density) {
        fog_density = scene->fog_density(dist, scene->fog_parameters);
    }
    
    
    Vector3d reflected_ray;
    if((material.Ks) || (material.Kr)) {        
        reflected_ray = reflect_ray(vector, norm);
    }
    
    // Ambient
    if(material.Ka) {
        ambient_color = mul_colors(scene->background_color, obj_color);
    }
    
    // Diffuse
    if(material.Kd) {
        diffuse_color = obj_color;
        
        if(scene->light_sources_count) {
            Color light_color = get_lighting_color(point, norm, scene);
            diffuse_color = mul_colors(diffuse_color, light_color);
        }
    }
    
    // Specular
    if(material.Ks) {
        specular_color = scene->background_color;
        
        if(scene->light_sources_count) {
            specular_color = get_specular_color(point, reflected_ray, scene, material.p);
        }
    }
    
    
    // Reflect
    if(material.Kr) {
        // Avoid deep recursion by tracing rays, which have intensity is greather than threshold
        // and avoid infinite recursion by limiting number of recursive calls
        if((intensity > THRESHOLD_RAY_INTENSITY)
           && (recursion_level < MAX_RAY_RECURSION_LEVEL)) {
            
            trace_recursively(scene,
                              point,
                              reflected_ray,
                              &reflected_color,
                              intensity * material.Kr * (1 - fog_density),
                              recursion_level + 1);
        } else {
            reflected_color = scene->background_color;
        }
    }
    
    // Result
    Color result_color = rgb(0, 0, 0);
    if(material.Ka) {
        result_color = add_colors(result_color,
                                  mul_color(ambient_color, material.Ka));
    }
    if(material.Kd) {
        result_color = add_colors(result_color,
                                  mul_color(diffuse_color, material.Kd));
    }
    if(material.Ks) {
        result_color = add_colors(result_color,
                                  mul_color(specular_color, material.Ks));
    }
    if(material.Kr) {
        result_color = add_colors(result_color,
                                  mul_color(reflected_color, material.Kr));
    }
    
    if(scene->fog_density) {
        result_color = add_colors(
                                  mul_color(scene->background_color, fog_density),
                                  mul_color(result_color, 1 - fog_density));
    }

    return result_color;
}

static inline Color
get_lighting_color(Point3d point,
                   Vector3d norm_v,
                   Scene * scene) {
    
    Color light_color = rgb(0, 0, 0);
    
    normalize_vector(&norm_v);
    
    LightSource3d * ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    int i;
    
    for(i = 0; i < scene->last_light_source_index + 1; i++) {
        if(scene->light_sources[i]) {
            ls = scene->light_sources[i];
        
            // If not shaded
            if(is_viewable(ls->location, point, scene)) {
                v_ls = vector3dp(point, ls->location);
                normalize_vector(&v_ls);
            
                cos_ls = fabs(cos_vectors(norm_v, v_ls));
                color_ls = mul_color(ls->color, cos_ls);
                light_color = add_colors(light_color, color_ls);
            }
        }
    }
    
    return light_color;
}

static inline Color
get_specular_color(Point3d point,
                   Vector3d reflected_ray,
                   Scene * scene,
                   Float p) {
    
    Color light_color = rgb(0, 0, 0);
    
    normalize_vector(&reflected_ray);
    
    LightSource3d * ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    int i;
    
    for(i = 0; i < scene->last_light_source_index + 1; i++) {
        if(scene->light_sources[i]) {
            ls = scene->light_sources[i];
        
            // If not shaded
            if(is_viewable(ls->location, point, scene)) {
                v_ls = vector3dp(point, ls->location);
                normalize_vector(&v_ls);
            
                cos_ls = cos_vectors(reflected_ray, v_ls);
                if(cos_ls <= EPSILON)
                    cos_ls = 0;
                color_ls = mul_color(ls->color, pow(cos_ls, p));
                light_color = add_colors(light_color, color_ls);
            }
        }
    }
    
    return light_color;
}

static inline Boolean
is_viewable(Point3d target_point,
            Point3d starting_point,
            Scene * scene) {
    
    Vector3d ray = vector3dp(starting_point, target_point);
    Float target_dist = module_vector(ray);
    
    normalize_vector(&ray);
    
    Object3d * nearest_obj = NULL;
    Point3d nearest_intersection_point;
    Float nearest_intersection_point_dist = FLOAT_MAX;
    
    if(find_intersection_tree(scene->kd_tree,
                              starting_point,
                              ray,
                              &nearest_obj,
                              &nearest_intersection_point,
                              &nearest_intersection_point_dist)) {

        // Check if intersection point is closer than target_point
        return (target_dist < nearest_intersection_point_dist);
    }
    // Ray doesn't intersect any of scene objects
    return True;
}