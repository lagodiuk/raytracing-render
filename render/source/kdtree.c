#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <render.h>
#include <kdtree.h>
#include <utils.h>

#define MAX_TREE_DEPTH 11

#define OBJECTS_IN_LEAF 6

#define __hot   __attribute__((hot))


// Declarations
// --------------------------------------------------------------

Voxel make_initial_voxel(Object3d ** objects,
                         int objects_count);

KDNode * rec_build(Object3d ** objects,
                   int objects_count,
                   Voxel v,
                   int iter);

Boolean terminate(Object3d ** objects,
                  int objects_count,
                  Voxel v,
                  int iter);

KDNode * make_leaf(Object3d ** objects,
                   int objects_count);

void find_plane(Object3d ** objects,
                int objects_count,
                Voxel v,
                enum Plane * p,
                Coord * c);

void split_voxel(Voxel v,
                 enum Plane p,
                 Coord c,
                 Voxel * vl,
                 Voxel * vr);

int filter_overlapped_objects(Object3d ** objects,
                              int objects_count,
                              Voxel v);

static inline Boolean vector_plane_intersection(Vector3d vector,
                                                Point3d vector_start,
                                                enum Plane plane,
                                                Coord coord,
                                                Point3d * result,
                                                Float * t);

static inline Boolean voxel_intersection(Vector3d vector,
                                         Point3d vector_start,
                                         Voxel v,
                                         Float * t_near,
                                         Float * t_far);

static inline Boolean object_in_voxel(Object3d * obj,
                                      Voxel v);

static inline Boolean point_in_voxel(Point3d p,
                                     Voxel v);

Boolean find_intersection_node(KDNode * node,
                               Voxel v,
                               Point3d vector_start,
                               Vector3d vector,
                               Object3d ** nearest_obj_ptr,
                               Point3d * nearest_intersection_point_ptr,
                               Float * nearest_intersection_point_dist_ptr);

Boolean is_intersect_anything_node(KDNode * node,
                                   Voxel v,
                                   Point3d vector_start,
                                   Vector3d vector);

void release_kd_node(KDNode * node);


// Code
// --------------------------------------------------------------

static inline Boolean point_in_voxel(Point3d p,
                                     Voxel v) {
    
    return ((p.x > v.x_min - EPSILON) && (p.x < v.x_max + EPSILON) &&
            (p.y > v.y_min - EPSILON) && (p.y < v.y_max + EPSILON) &&
            (p.z > v.z_min - EPSILON) && (p.z < v.z_max + EPSILON));
}


inline void release_kd_tree(KDTree * tree) {
    release_kd_node(tree->root);
    free(tree);
}

void release_kd_node(KDNode * node) {
    if(node->l)
        release_kd_node(node->l);
    if(node->r)
        release_kd_node(node->r);
    if(node->objects)
        free(node->objects);
}

inline KDTree * build_kd_tree(Object3d ** objects,
                              int objects_count) {
    
    KDTree * tree = malloc(sizeof(KDTree));
    tree->bounding_box = make_initial_voxel(objects, objects_count);
    tree->root = rec_build(objects, objects_count, tree->bounding_box, 0);
    return tree;
}

KDNode * rec_build(Object3d ** objects,
                   int objects_count,
                   Voxel v,
                   int iter) {
    
    if(terminate(objects, objects_count, v, iter)) {
        return make_leaf(objects, objects_count);
    }
    
    enum Plane p;
    Coord c;
    find_plane(objects, objects_count, v, &p, &c);
    
    Voxel vl;
    Voxel vr;
    split_voxel(v, p, c, &vl, &vr);
    
    int l_objects_count = filter_overlapped_objects(objects, objects_count, vl);
    KDNode * l = rec_build(objects, l_objects_count, vl, iter + 1);
    
    int r_objects_count = filter_overlapped_objects(objects, objects_count, vr);
    KDNode * r = rec_build(objects, r_objects_count, vr, iter + 1);

    
    KDNode * node = malloc(sizeof(KDNode));
    node->objects = NULL;
    node->objects_count = 0;
    node->plane = p;
    node->coord = c;
    node->l = l;
    node->r = r;
    
    return node;
}

int filter_overlapped_objects(Object3d ** objects,
                              int objects_count,
                              Voxel v) {
    
    int i = 0;
    int j = objects_count - 1;
    int overlapped_count = 0;
    
    Object3d * tmp;
    
    // Put all objects, which overlap with voxel to the left part of array
    while(i <= j) {
        while((i < j) && (object_in_voxel(objects[i], v))) {
            i++;
            overlapped_count++;
        }
        
        while((j > i) && (!object_in_voxel(objects[j], v)))
            j--;
        
        tmp = objects[i];
        objects[i] = objects[j];
        objects[j] = tmp;
        i++;
        j--;
        overlapped_count++;
    }
     
    return overlapped_count;
}

void split_voxel(Voxel v,
                 enum Plane p,
                 Coord c,
                 Voxel * vl,
                 Voxel * vr) {
    
    *vl = v;
    *vr = v;
    
    switch(p) {
        case XY:
            vl->z_max = c.z;
            vr->z_min = c.z;
            break;
            
        case XZ:
            vl->y_max = c.y;
            vr->y_min = c.y;
            break;
            
        case YZ:
            vl->x_max = c.x;
            vr->x_min = c.x;
            break;
    }
}

void find_plane(Object3d ** objects,
                int objects_count,
                Voxel v,
                enum Plane * p,
                Coord * c) {
    
    // TODO use Surface Area Heuristic (SAH)
    Float dx = v.x_max - v.x_min;
    Float dy = v.y_max - v.y_min;
    Float dz = v.z_max - v.z_min;

    if((dx >= dy) && (dx >= dz)) {
        *p = YZ;
        c->x = v.x_min + dx / 2.0;
        return;
    } else if((dy >= dx) && (dy >= dz)) {
        *p = XZ;
        c->y = v.y_min + dy / 2.0;
        return;
    } else {
        *p = XY;
        c->z = v.z_min + dz / 2.0;
        return;
    }
}

Boolean terminate(Object3d ** objects,
                  int objects_count,
                  Voxel v,
                  int iter) {
    
    // TODO use SAH
    if((iter < MAX_TREE_DEPTH) && (objects_count > OBJECTS_IN_LEAF)) {
        return False;
    }
    return True;
}

Voxel make_initial_voxel(Object3d ** objects,
                         int objects_count) {
    
    if(!objects_count) {
        Voxel v = {-1, -1, -1, 1, 1, 1};
        return v;
    }
        
    Point3d min_p = objects[0]->get_min_boundary_point(objects[0]->data);
    Point3d max_p = objects[0]->get_max_boundary_point(objects[0]->data);
    
    Float x_min = min_p.x;
    Float y_min = min_p.y;
    Float z_min = min_p.z;
    
    Float x_max = max_p.x;
    Float y_max = max_p.y;
    Float z_max = max_p.z;
    
    int i;
    for(i = 0; i < objects_count; i++) {
        min_p = objects[i]->get_min_boundary_point(objects[i]->data);
        max_p = objects[i]->get_max_boundary_point(objects[i]->data);
        
        x_min = (x_min < min_p.x) ? x_min : min_p.x;
        y_min = (y_min < min_p.y) ? y_min : min_p.y;
        z_min = (z_min < min_p.z) ? z_min : min_p.z;
        
        x_max = (x_max > max_p.x) ? x_max : max_p.x;
        y_max = (y_max > max_p.y) ? y_max : max_p.y;
        z_max = (z_max > max_p.z) ? z_max : max_p.z;
    }
    
    Voxel v = {x_min - 1, y_min - 1, z_min - 1, x_max + 1, y_max + 1, z_max + 1};
    return v;
}


static inline __hot Boolean object_in_voxel(Object3d * obj,
                                            Voxel v) {
    
    Point3d min_p = obj->get_min_boundary_point(obj->data);
    Point3d max_p = obj->get_max_boundary_point(obj->data);

    if((max_p.x < v.x_min)
       || (max_p.y < v.y_min)
       || (max_p.z < v.z_min)
       || (min_p.x > v.x_max)
       || (min_p.y > v.y_max)
       || (min_p.z > v.z_max)) return False;
    
    return True;
}

KDNode * make_leaf(Object3d ** objects,
                   int objects_count) {
    
    KDNode * leaf = malloc(sizeof(KDNode));
    leaf->plane = NONE;
    leaf->objects_count = objects_count;
    leaf->l = NULL;
    leaf->r = NULL;
    if(objects_count) {
        leaf->objects = (Object3d **) calloc(objects_count, sizeof(Object3d *));
    } else {
        leaf->objects = NULL;
    }
    
    int i;
    if(objects_count) {
        for(i = 0; i < objects_count; i++) {
            leaf->objects[i] = objects[i];
        }
    }
    return leaf;
}

static inline __hot Boolean vector_plane_intersection(Vector3d vector,
                                                      Point3d vector_start,
                                                      enum Plane plane,
                                                      Coord coord,
                                                      Point3d * result,
                                                      Float * t) {
    Float k;
    
    switch(plane) {
        case XY:
            k = (coord.z - vector_start.z) / vector.z;
            break;
        
        case XZ:
            k = (coord.y - vector_start.y) / vector.y;
            break;
            
        case YZ:
            k = (coord.x - vector_start.x) / vector.x;
            break;
    }
    
    *result = point3d(vector_start.x + vector.x * k,
                      vector_start.y + vector.y * k,
                      vector_start.z + vector.z * k);
    
    *t = k;
    
    return True;
}

static inline Boolean voxel_intersection(Vector3d vector,
                                         Point3d vector_start,
                                         Voxel v,
                                         Float * t_near,
                                         Float * t_far) {
    
    Float t_min;
    Float t_max;
    int intersected = False;
    
    Point3d p;
    Float t;
    Coord c;
    
    c.z = v.z_min;
    if(vector_plane_intersection(vector, vector_start, XY, c, &p, &t)
       && point_in_voxel(p, v)) {
        
        if(!intersected) {
            t_min = t;
            t_max = t;
        } else {
            t_min = (t < t_min) ? t : t_min;
            t_max = (t > t_max) ? t : t_max;
        }
        intersected = True;
    }
    
    c.z = v.z_max;
    if(vector_plane_intersection(vector, vector_start, XY, c, &p, &t)
       && point_in_voxel(p, v)) {
        
        if(!intersected) {
            t_min = t;
            t_max = t;
        } else {
            t_min = (t < t_min) ? t : t_min;
            t_max = (t > t_max) ? t : t_max;
        }
        intersected = True;
    }
    
    c.y = v.y_min;
    if(vector_plane_intersection(vector, vector_start, XZ, c, &p, &t)
       && point_in_voxel(p, v)) {
        
        if(!intersected) {
            t_min = t;
            t_max = t;
        } else {
            t_min = (t < t_min) ? t : t_min;
            t_max = (t > t_max) ? t : t_max;
        }
        intersected = True;
    }
    
    c.y = v.y_max;
    if(vector_plane_intersection(vector, vector_start, XZ, c, &p, &t)
       && point_in_voxel(p, v)) {
        
        if(!intersected) {
            t_min = t;
            t_max = t;
        } else {
            t_min = (t < t_min) ? t : t_min;
            t_max = (t > t_max) ? t : t_max;
        }
        intersected = True;
    }
    
    c.x = v.x_min;
    if(vector_plane_intersection(vector, vector_start, YZ, c, &p, &t)
       && point_in_voxel(p, v)) {
        
        if(!intersected) {
            t_min = t;
            t_max = t;
        } else {
            t_min = (t < t_min) ? t : t_min;
            t_max = (t > t_max) ? t : t_max;
        }
        intersected = True;
    }
    
    c.x = v.x_max;
    if(vector_plane_intersection(vector, vector_start, YZ, c, &p, &t)
       && point_in_voxel(p, v)) {
        
        if(!intersected) {
            t_min = t;
            t_max = t;
        } else {
            t_min = (t < t_min) ? t : t_min;
            t_max = (t > t_max) ? t : t_max;
        }
        intersected = True;
    }
    
    if(intersected) {
        *t_near = t_min;
        *t_far = t_max;
    }
    
    return intersected;
}

Boolean find_intersection_tree(KDTree * tree,
                               Point3d vector_start,
                               Vector3d vector,
                               Object3d ** nearest_obj_ptr,
                               Point3d * nearest_intersection_point_ptr,
                               Float * nearest_intersection_point_dist_ptr) {
    
    return find_intersection_node(tree->root,
                                  tree->bounding_box,
                                  vector_start,
                                  vector,
                                  nearest_obj_ptr,
                                  nearest_intersection_point_ptr,
                                  nearest_intersection_point_dist_ptr);
}

Boolean find_intersection_node(KDNode * node,
                               Voxel v,
                               Point3d vector_start,
                               Vector3d vector,
                               Object3d ** nearest_obj_ptr,
                               Point3d * nearest_intersection_point_ptr,
                               Float * nearest_intersection_point_dist_ptr) {
    
    Float t_near;
    Float t_far;
    
    if(!voxel_intersection(vector, vector_start, v, &t_near, &t_far)) {
        return False;
    }
    
    if((t_near < 0) && (t_far < 0)) {
        return False;
    }
    
    // Is leaf
    if(node->plane == NONE) {
        if((node->objects_count) && (node->objects)) {
            int i;
            Object3d * obj = NULL;
            Point3d intersection_point;
            Float curr_intersection_point_dist;
            int intersected = False;
            
            // Finding nearest object
            // and intersection point
            for(i = 0; i < node->objects_count; i++) {
                if(node->objects[i]) {
                    obj = node->objects[i];
                    
                    if((obj->intersect(obj->data, vector_start, vector, &intersection_point))
                       && (point_in_voxel(intersection_point, v))) {

                        curr_intersection_point_dist = module_vector(vector3dp(vector_start, intersection_point));
                        
                        if(curr_intersection_point_dist < *nearest_intersection_point_dist_ptr) {
                            *nearest_obj_ptr = obj;
                            *nearest_intersection_point_ptr = intersection_point;
                            *nearest_intersection_point_dist_ptr = curr_intersection_point_dist;
                            intersected = True;
                        }
                    }
                }
            }
            
            return intersected;
        }
        return False;
    }

    // Otherwise    
    
    Voxel front_voxel;
    Voxel back_voxel;
        
    KDNode * front_node;
    KDNode * back_node;
    
    Voxel v_l;
    Voxel v_r;
    split_voxel(v, node->plane, node->coord, &v_l, &v_r);

    switch(node->plane) {
        case XY:
            if((node->coord.z - v_l.z_min) * (node->coord.z - vector_start.z) > 0) {
                front_node = node->l;
                back_node = node->r;
                front_voxel = v_l;
                back_voxel = v_r;
            } else {
                front_node = node->r;
                back_node = node->l;
                front_voxel = v_r;
                back_voxel = v_l;
            }
            break;
                
        case XZ:
            if((node->coord.y - v_l.y_min) * (node->coord.y - vector_start.y) > 0) {
                front_node = node->l;
                back_node = node->r;
                front_voxel = v_l;
                back_voxel = v_r;
            } else {
                front_node = node->r;
                back_node = node->l;
                front_voxel = v_r;
                back_voxel = v_l;
            }
            break;
            
        case YZ:
            if((node->coord.x - v_l.x_min) * (node->coord.x - vector_start.x) > 0) {
                front_node = node->l;
                back_node = node->r;
                front_voxel = v_l;
                back_voxel = v_r;
            } else {
                front_node = node->r;
                back_node = node->l;
                front_voxel = v_r;
                back_voxel = v_l;
            }
            break;
    }    

    if(find_intersection_node(front_node,
                              front_voxel,
                              vector_start,
                              vector,
                              nearest_obj_ptr,
                              nearest_intersection_point_ptr,
                              nearest_intersection_point_dist_ptr)) return True;

        
    return find_intersection_node(back_node,
                                  back_voxel,
                                  vector_start,
                                  vector,
                                  nearest_obj_ptr,
                                  nearest_intersection_point_ptr,
                                  nearest_intersection_point_dist_ptr);
}

Boolean is_intersect_anything_tree(KDTree * tree,
                               Point3d vector_start,
                               Vector3d vector) {

    return is_intersect_anything_node(tree->root,
                                      tree->bounding_box,
                                      vector_start,
                                      vector);
}

Boolean is_intersect_anything_node(KDNode * node,
                               Voxel v,
                               Point3d vector_start,
                               Vector3d vector) {
    
    Float t_near;
    Float t_far;
    
    if(!voxel_intersection(vector, vector_start, v, &t_near, &t_far)) {
        return False;
    }
    
    if((t_near < 0) && (t_far < 0)) {
        return False;
    }
    
    // Is leaf
    if(node->plane == NONE) {
        if((node->objects_count) && (node->objects)) {
            int i;
            Point3d intersection_point;
            Object3d * obj;
            
            // Finding nearest object
            // and intersection point
            for(i = 0; i < node->objects_count; i++) {
                if(node->objects[i]) {
                    obj = node->objects[i];
                    
                    if((obj->intersect(obj->data, vector_start, vector, &intersection_point))
                       && (point_in_voxel(intersection_point, v))) {
                        
                        return True;
                    }
                }
            }
        }
        return False;
    }
    
    // Otherwise
    
    Voxel v_l;
    Voxel v_r;    
    split_voxel(v, node->plane, node->coord, &v_l, &v_r);
        
    if(is_intersect_anything_node(node->l, v_l, vector_start, vector)
       || is_intersect_anything_node(node->r, v_r, vector_start, vector)) return True;

    return False;
}
