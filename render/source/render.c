#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <render.h>
#include <utils.h>

#define INITIAL_RAY_INTENSITY 100
#define THRESHOLD_RAY_INTENSITY 10
#define MAX_RAY_ITERATIONS 10

int is_viewable(Point3d target_point, Point3d starting_point, Scene * scene);

Color get_lighting_color(Point3d point, Vector3d norm_v, Scene * scene);

Color get_specular_color(Point3d point, Vector3d reflected_ray, Scene * scene, Float p);

inline Vector3d reflect_ray(Vector3d incident_ray, Vector3d norm_v);

inline Float exponential_fog_density(Float distance, void * fog_data);

void trace_i(Scene * scene,
             Point3d vector_start,
             Vector3d vector,
             Color * color,
             Float intensity,
             int iteration_num);

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

inline LightSource3d * light_source_3d(Point3d location, Color color) {
	LightSource3d * ls_p = malloc(sizeof(LightSource3d));
    
    ls_p->location_world = location;
    ls_p->location = location;
    ls_p->color = color;
    
	return ls_p;
}

inline Material material(Float Ka, Float Kd, Float Ks, Float Kr, Float Kt, Float p) {
    Float sum = Ka + Kd + Ks + Kr + Kt;
    Material m = {.Ka = Ka / sum,
                  .Kd = Kd / sum,
                  .Ks = Ks / sum,
                  .Kr = Kr / sum,
                  .Kt = Kt / sum,
                  .p = p};
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
        s->light_sources = calloc(light_sources_count, sizeof(LightSource3d *));
    }
    s->light_sources_count = light_sources_count;
    s->background_color = background_color;
    s->last_object_index = -1;
    s->last_light_source_index = -1;
    s->fog_parameters = NULL;
    s->fog_density = NULL;
    return s;
}

inline void release_scene(Scene * scene) {
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
}

inline void add_object(Scene * scene, Object3d * object) {
    scene->objects[++scene->last_object_index] = object;
}

inline void add_light_source(Scene * scene, LightSource3d * light_source) {
    scene->light_sources[++scene->last_light_source_index] = light_source;
}

inline void set_exponential_fog(Scene * scene, Float k) {
    scene->fog_density = exponential_fog_density;
    
    Float * k_p = malloc(sizeof(Float));
    *k_p = k;
    
    if(scene->fog_parameters) {
        free(scene->fog_parameters);
    }
    scene->fog_parameters = k_p;
}

inline void set_no_fog(Scene * scene) {
    if(scene->fog_parameters) {
        free(scene->fog_parameters);
    }
    scene->fog_density = NULL;
}

inline Float exponential_fog_density(Float distance, void * fog_data) {
    Float * k = (Float *) fog_data;
    return 1 - exp(- (*k) * distance);
}

void trace(Scene * scene,
           Point3d vector_start,
           Vector3d vector,
           Color * color) {
    
    trace_i(scene,
            vector_start,
            vector,
            color,
            INITIAL_RAY_INTENSITY,
            1);
}

void trace_i(Scene * scene,
             Point3d vector_start,
             Vector3d vector,
             Color * color,
             Float intensity,
             int iteration_num) {

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
    for(i = 0; i < scene->last_object_index + 1; i++) {
        if(scene->objects[i]) {
            obj = scene->objects[i];
        
            if(obj->intersect(obj->data, vector_start, vector, &intersection_point)) {
                curr_intersection_point_dist = module_vector(vector3dp(vector_start, intersection_point));
            
                if(curr_intersection_point_dist < nearest_intersection_point_dist) {
                    nearest_obj = obj;
                    nearest_intersection_point = intersection_point;
                    nearest_intersection_point_dist = curr_intersection_point_dist;
                }
            }
        }
    }

    if(nearest_obj) {
        Material material = nearest_obj->get_material(nearest_obj->data,
                                                      nearest_intersection_point);
        
        Vector3d norm = nearest_obj->get_normal_vector(nearest_obj->data, nearest_intersection_point);
        
        Color obj_color = nearest_obj->get_color(nearest_obj->data,
                                                 nearest_intersection_point);
        Color ambient_color;
        Color diffuse_color;
        Color reflected_color;
        Color specular_color;
        
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
               Color light_color = get_lighting_color(nearest_intersection_point, norm, scene);
               diffuse_color = mul_colors(diffuse_color, light_color);
            }
       }
            
        // Specular
        if(material.Ks) {
            specular_color = scene->background_color;
            if(scene->light_sources_count) {
                specular_color = get_specular_color(nearest_intersection_point, reflected_ray, scene, material.p);
            }
        }
        
        
        // Reflect
        if(material.Kr) {
            // Avoid deep recursion by tracing rays, which have intensity is greather than threshold
            // and avoid infinite recursion by limiting number of recursive calls
            if((intensity > THRESHOLD_RAY_INTENSITY) && (iteration_num < MAX_RAY_ITERATIONS)) {
                trace_i(scene,
                        nearest_intersection_point,
                        reflected_ray,
                        &reflected_color,
                        intensity * material.Kr,
                        iteration_num + 1);
            } else {
                reflected_color = scene->background_color;
            }
        }
        
        // Result
        Color result_color = rgb(0, 0, 0);
        if(material.Ka) {
            result_color = add_colors(result_color, mul_color(ambient_color, material.Ka));
        }
        if(material.Kd) {
            result_color = add_colors(result_color, mul_color(diffuse_color, material.Kd));
        }
        if(material.Ks) {
            result_color = add_colors(result_color, mul_color(specular_color, material.Ks));
        }
        if(material.Kr) {
            result_color = add_colors(result_color, mul_color(reflected_color, material.Kr));
        }
        
        if(scene->fog_density) {
            Float distance_to_intersection = module_vector(vector3dp(vector_start, nearest_intersection_point));
            Float fog_density = scene->fog_density(distance_to_intersection, scene->fog_parameters);
            result_color = add_colors(
                                      mul_color(scene->background_color, fog_density),
                                      mul_color(result_color, 1 - fog_density));
        }
        
        *color = result_color;
        return;
    }
    
    *color = scene->background_color;
}

Color get_lighting_color(Point3d point, Vector3d norm_v, Scene * scene) {
    Color light_color = rgb(0, 0, 0);
    
    normalize_vector(&norm_v);
    
    LightSource3d * ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    int i;
    
    for(i = 0; i < scene->last_light_source_index; i++) {
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

Color get_specular_color(Point3d point, Vector3d reflected_ray, Scene * scene, Float p) {
    Color light_color = rgb(0, 0, 0);
    
    normalize_vector(&reflected_ray);
    
    LightSource3d * ls;
    Vector3d v_ls;
    Float cos_ls;
    Color color_ls;
    int i;
    
    for(i = 0; i < scene->last_light_source_index; i++) {
        if(scene->light_sources[i]) {
            ls = scene->light_sources[i];
        
            // If not shaded
            if(is_viewable(ls->location, point, scene)) {
                v_ls = vector3dp(point, ls->location);
                normalize_vector(&v_ls);
            
                cos_ls = fabs(cos_vectors(reflected_ray, v_ls));
                color_ls = mul_color(ls->color, pow(cos_ls, p));
                light_color = add_colors(light_color, color_ls);
            }
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
    for(i = 0; i < scene->last_object_index; i++) {
        if(scene->objects[i]) {
            obj = scene->objects[i];
        
            if(obj->intersect(obj->data, starting_point, ray, &intersection_point)) {
                // Target point is not viewable from starting point
                // because of ray intersects some of scene objects
                return 0;
            }
        }
    }
    
    // Ray doesn't intersect any of scene objects
    return 1;
}

inline Vector3d reflect_ray(Vector3d incident_ray, Vector3d norm_v) {
    Float numerator = 2 * (incident_ray.x * norm_v.x + incident_ray.y * norm_v.y + incident_ray.z * norm_v.z);
    
    Float norm_module = module_vector(norm_v);
    Float denominator = norm_module * norm_module;
    
    Float k = numerator / denominator;
    
    Float x = incident_ray.x - norm_v.x * k;
    Float y = incident_ray.y - norm_v.y * k;
    Float z = incident_ray.z - norm_v.z * k;
    
    return vector3df(x, y, z);
}
