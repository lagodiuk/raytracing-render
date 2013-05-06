#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include <render.h>
#include <kdtree.h>
#include <utils.h>

#define MAX_TREE_DEPTH 15

#define OBJECTS_IN_LEAF 3

#define MAX_SPLITS_OF_VOXEL 20

#define SPLIT_COST 15

#if defined(__GNUC__) && (__GNUC__ * 100 +  __GNUC_MINOR__) >= 403
# define __hot   __attribute__((hot))
#else
# define __hot
#endif


// Declarations
// --------------------------------------------------------------

Voxel
make_initial_voxel(Object3d ** objects,
                   int objects_count);

inline KDNode *
rec_build(Object3d ** objects,
          int objects_count,
          Voxel v,
          int iter);

inline KDNode *
make_leaf(Object3d ** objects,
          int objects_count);

static inline void
find_plane(Object3d ** objects,
           const int objects_count,
           const Voxel v,
           const int tree_depth,
           enum Plane * const p,
           Coord * const c);

static inline int
objects_in_voxel(Object3d ** objects,
                 const int objects_count,
                 const Voxel v);

static inline void
split_voxel(const Voxel v,
            const enum Plane p,
            const Coord c,
            Voxel * const vl,
            Voxel * const vr);

static inline int
filter_overlapped_objects(Object3d ** objects,
                          const int objects_count,
                          const Voxel v);

static inline Boolean
vector_plane_intersection(const Vector3d vector,
                          const Point3d vector_start,
                          const enum Plane plane,
                          const Coord coord,
                          Point3d * const result,
                          Float * const t);

static inline Boolean
voxel_intersection(const Vector3d vector,
                   const Point3d vector_start,
                   const Voxel v,
                   Float * const t_near,
                   Float * const t_far);

static inline Boolean
object_in_voxel(Object3d * const obj,
                const Voxel v);

static inline Boolean
point_in_voxel(const Point3d p,
               const Voxel v);

inline Boolean
find_intersection_node(KDNode * const node,
                       const Voxel v,
                       const Point3d vector_start,
                       const Vector3d vector,
                       Object3d ** const nearest_obj_ptr,
                       Point3d * const nearest_intersection_point_ptr,
                       Float * const nearest_intersection_point_dist_ptr);

inline Boolean
is_intersect_anything_node(KDNode * const node,
                           const Voxel v,
                           const Point3d vector_start,
                           const Vector3d vector);

void
release_kd_node(KDNode * node);


// Code
// --------------------------------------------------------------

static inline Boolean
point_in_voxel(const Point3d p,
               const Voxel v) {
    
    return ((p.x > v.x_min - EPSILON) && (p.x < v.x_max + EPSILON) &&
            (p.y > v.y_min - EPSILON) && (p.y < v.y_max + EPSILON) &&
            (p.z > v.z_min - EPSILON) && (p.z < v.z_max + EPSILON));
}


void
release_kd_tree(KDTree * tree) {
    release_kd_node(tree->root);
    free(tree);
}

void
release_kd_node(KDNode * node) {
    if(node->l)
        release_kd_node(node->l);
    if(node->r)
        release_kd_node(node->r);
    if(node->objects)
        free(node->objects);
    free(node);
}

KDTree *
build_kd_tree(Object3d ** objects,
              int objects_count) {
    
    KDTree * tree = malloc(sizeof(KDTree));
    tree->bounding_box = make_initial_voxel(objects, objects_count);
    tree->root = rec_build(objects, objects_count, tree->bounding_box, 0);
    return tree;
}

inline KDNode *
rec_build(Object3d ** objects,
          int objects_count,
          Voxel v,
          int iter) {
    
    enum Plane p;
    Coord c;
    find_plane(objects, objects_count, v, iter, &p, &c);
    
    if(p == NONE) {
        return make_leaf(objects, objects_count);
    }
    
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

static inline int
filter_overlapped_objects(Object3d ** objects,
                          const int objects_count,
                          const Voxel v) {
    
    int i = 0;
    int j = objects_count - 1;
    
    Object3d * tmp;
    
    // Put all objects, which overlap with voxel to the left part of array
    while(i <= j) {
        while((i < j) && (object_in_voxel(objects[i], v)))
            i++;
        
        while((j > i) && (!object_in_voxel(objects[j], v)))
            j--;
        
        tmp = objects[i];
        objects[i] = objects[j];
        objects[j] = tmp;
        i++;
        j--;
    }
    
    // Actually, after the loop, variable 'i' is a count of overlapped objects
    return i;
}

static inline void
split_voxel(const Voxel v,
            const enum Plane p,
            const Coord c,
            Voxel * const vl,
            Voxel * const vr) {
    
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

/*
 * Using Surface Area Heuristic (SAH) for finding best split pane
 *
 * SAH = voxel_surface_area * number_of_objects_in_voxel
 *
 * splitted_SAH = split_cost
 *                + 0.5 * left_voxel_surface_area * number_of_objects_in_left_voxel
 *                + 0.5 * right_voxel_surface_area * number_of_objects_in_right_voxel
 *
 *
 * see: http://stackoverflow.com/a/4633332/653511
 */
static inline void
find_plane(Object3d ** objects,
           const int objects_count,
           const Voxel v,
           const int tree_depth,
           enum Plane * const p,
           Coord * const c) {
    
    if((tree_depth >= MAX_TREE_DEPTH) || (objects_count <= OBJECTS_IN_LEAF)) {
        *p = NONE;
        return;
    }
    
    Float hx = v.x_max - v.x_min;
    Float hy = v.y_max - v.y_min;
    Float hz = v.z_max - v.z_min;
    
    // Calculating square of each side of initial voxel
    Float Sxy = hx * hy;
    Float Sxz = hx * hz;
    Float Syz = hy * hz;
    
    Float Ssum = Sxy + Sxz + Syz;

    // Let's normalize square of each side of initial voxel
    // to satisfy the following relationship:
    // Sxy + Sxz + Syz = 1
    Sxy /= Ssum;
    Sxz /= Ssum;
    Syz /= Ssum;
    
    int max_splits = MAX_SPLITS_OF_VOXEL;
    Float split_cost = SPLIT_COST;
    
    // Assume that at the beginning best SAH has initial voxel
    // SAH = 0.5 * square * objects_count
    // squre of initial voxel is Sxy + Sxz + Syz = 1
    Float bestSAH = objects_count;
    // initial voxel doesn't have split pane
    *p = NONE;
    
    Float currSAH;
    Coord curr_split_coord;
    int i;
    Voxel vl;
    Voxel vr;
    Float a;
    
    // Let's find split surface, which have the least SAH
    
    // XY
    for(i = 1; i < max_splits; i++) {

        a = ((float) i) / max_splits;
        
        // Current coordinate of split surface
        curr_split_coord.z = v.z_min + a * hz;
        
        split_voxel(v, XY, curr_split_coord, &vl, &vr);
        
        currSAH = (Sxy +      a  * (Sxz + Syz)) * objects_in_voxel(objects, objects_count, vl)
                + (Sxy + (1 - a) * (Sxz + Syz)) * objects_in_voxel(objects, objects_count, vr) + split_cost;
        
        if(currSAH < bestSAH) {
            bestSAH = currSAH;
            *p = XY;
            *c = curr_split_coord;
        }
    }
    
    // XZ
    for(i = 1; i < max_splits; i++) {
        
        a = ((float) i) / max_splits;

        // Current coordinate of split surface       
        curr_split_coord.y = v.y_min + a * hy;
        
        split_voxel(v, XZ, curr_split_coord, &vl, &vr);
        
        currSAH = (Sxz +      a  * (Sxy + Syz)) * objects_in_voxel(objects, objects_count, vl)
                + (Sxz + (1 - a) * (Sxy + Syz)) * objects_in_voxel(objects, objects_count, vr) + split_cost;
        
        if(currSAH < bestSAH) {
            bestSAH = currSAH;
            *p = XZ;
            *c = curr_split_coord;
        }
    }
    
    // YZ
    for(i = 1; i < max_splits; i++) {
        
        a = ((float) i) / max_splits;
        
        // Current coordinate of split surface
        curr_split_coord.x = v.x_min + a * hx;
        
        split_voxel(v, YZ, curr_split_coord, &vl, &vr);
        
        currSAH = (Syz +      a  * (Sxy + Sxz)) * objects_in_voxel(objects, objects_count, vl)
                + (Syz + (1 - a) * (Sxy + Sxz)) * objects_in_voxel(objects, objects_count, vr) + split_cost;
        
        if(currSAH < bestSAH) {
            bestSAH = currSAH;
            *p = YZ;
            *c = curr_split_coord;
        }
    }
}

static inline int
objects_in_voxel(Object3d ** objects,
                 const int objects_count,
                 const Voxel v) {
    
    int i;
    int count = 0;
    for(i = 0; i < objects_count; i++)
        if(object_in_voxel(objects[i], v))
            ++count;
    
    return count;
}

Voxel
make_initial_voxel(Object3d ** objects,
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


static inline __hot Boolean
object_in_voxel(Object3d * const obj,
                const Voxel v) {
    
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

inline KDNode *
make_leaf(Object3d ** objects,
          int objects_count) {
    
    KDNode * leaf = malloc(sizeof(KDNode));
    leaf->plane = NONE;
    leaf->objects_count = objects_count;
    leaf->l = NULL;
    leaf->r = NULL;
    if(objects_count) {
        leaf->objects = (Object3d **) malloc(objects_count * sizeof(Object3d *));
    } else {
        leaf->objects = NULL;
    }
    
    memcpy(leaf->objects, objects, objects_count * sizeof(Object3d *));
    
    return leaf;
}

static inline __hot Boolean
vector_plane_intersection(const Vector3d vector,
                          const Point3d vector_start,
                          const enum Plane plane,
                          const Coord coord,
                          Point3d * const result,
                          Float * const t) {
    
    Float k;    
    switch(plane) {
        case XY:
            k = (coord.z - vector_start.z) / vector.z;
            *result = point3d(vector_start.x + vector.x * k,
                              vector_start.y + vector.y * k,
                              coord.z);
            break;
        
        case XZ:
            k = (coord.y - vector_start.y) / vector.y;
            *result = point3d(vector_start.x + vector.x * k,
                              coord.y,
                              vector_start.z + vector.z * k);
            break;
            
        case YZ:
            k = (coord.x - vector_start.x) / vector.x;
            *result = point3d(coord.x,
                              vector_start.y + vector.y * k,
                              vector_start.z + vector.z * k);
            break;
    }
    
    *t = k;
    
    return True;
}

static inline Boolean
voxel_intersection(const Vector3d vector,
                   const Point3d vector_start,
                   const Voxel v,
                   Float * const t_near,
                   Float * const t_far) {
    
    Float t_min;
    Float t_max;
    int intersected = False;
    
    Point3d p;
    Float t;
    Coord c;
    
    c.z = v.z_min;
    if(vector_plane_intersection(vector, vector_start, XY, c, &p, &t)
       && (p.x > v.x_min - EPSILON) && (p.x < v.x_max + EPSILON)
       && (p.y > v.y_min - EPSILON) && (p.y < v.y_max + EPSILON)) {
        
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
       && (p.x > v.x_min - EPSILON) && (p.x < v.x_max + EPSILON)
       && (p.y > v.y_min - EPSILON) && (p.y < v.y_max + EPSILON)) {
        
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
       && (p.x > v.x_min - EPSILON) && (p.x < v.x_max + EPSILON)
       && (p.z > v.z_min - EPSILON) && (p.z < v.z_max + EPSILON)) {
        
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
       && (p.x > v.x_min - EPSILON) && (p.x < v.x_max + EPSILON)
       && (p.z > v.z_min - EPSILON) && (p.z < v.z_max + EPSILON)) {
        
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
       && (p.y > v.y_min - EPSILON) && (p.y < v.y_max + EPSILON)
       && (p.z > v.z_min - EPSILON) && (p.z < v.z_max + EPSILON)) {
        
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
       && (p.y > v.y_min - EPSILON) && (p.y < v.y_max + EPSILON)
       && (p.z > v.z_min - EPSILON) && (p.z < v.z_max + EPSILON)) {
        
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

Boolean
find_intersection_tree(KDTree * const tree,
                       const Point3d vector_start,
                       const Vector3d vector,
                       Object3d ** const nearest_obj_ptr,
                       Point3d * const nearest_intersection_point_ptr,
                       Float * const nearest_intersection_point_dist_ptr) {
    
    Float t_near;
    Float t_far;
    
    return (voxel_intersection(vector, vector_start, tree->bounding_box, &t_near, &t_far)
            && ((t_near >= 0) || (t_far >= 0))
            && find_intersection_node(tree->root,
                                      tree->bounding_box,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr));
}

inline Boolean
find_intersection_node(KDNode * const node,
                       const Voxel v,
                       const Point3d vector_start,
                       const Vector3d vector,
                       Object3d ** const nearest_obj_ptr,
                       Point3d * const nearest_intersection_point_ptr,
                       Float * const nearest_intersection_point_dist_ptr) {
        
    // Is leaf
    if(node->plane == NONE) {
        if((node->objects_count) && (node->objects)) {
            int i;
            
            Object3d * obj = NULL;
            Point3d intersection_point;
            Float sqr_curr_dist;
            
            Object3d * nearest_obj = NULL;
            Point3d nearest_intersection_point;
            Float sqr_nearest_dist;            
            
            int intersected = False;
            
            // Finding nearest object
            // and intersection point
            for(i = 0; i < node->objects_count; i++) {
                if(node->objects[i]) {
                    obj = node->objects[i];
                    
                    if((obj->intersect(obj->data, vector_start, vector, &intersection_point))
                       && (point_in_voxel(intersection_point, v))) {

                        sqr_curr_dist = sqr_module_vector(vector3dp(vector_start, intersection_point));
                        
                        if((sqr_curr_dist < sqr_nearest_dist) || (!intersected)) {
                            
                            nearest_obj = obj;
                            nearest_intersection_point = intersection_point;
                            sqr_nearest_dist = sqr_curr_dist;
                            intersected = True;
                        }
                    }
                }
            }
            
            if(intersected) {
                Float nearest_dist = sqrt(sqr_nearest_dist);
                
                if(nearest_dist < *nearest_intersection_point_dist_ptr) {
                    *nearest_intersection_point_dist_ptr = nearest_dist;
                    *nearest_obj_ptr = nearest_obj;
                    *nearest_intersection_point_ptr = nearest_intersection_point;
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

    switch(node->plane) {
        case XY:
            if((node->coord.z - v.z_min) * (node->coord.z - vector_start.z) > 0) {
                front_node = node->l;
                back_node = node->r;
                split_voxel(v, node->plane, node->coord, &front_voxel, &back_voxel);
            } else {
                front_node = node->r;
                back_node = node->l;
                split_voxel(v, node->plane, node->coord, &back_voxel, &front_voxel);
            }
            break;
                
        case XZ:
            if((node->coord.y - v.y_min) * (node->coord.y - vector_start.y) > 0) {
                front_node = node->l;
                back_node = node->r;
                split_voxel(v, node->plane, node->coord, &front_voxel, &back_voxel);
            } else {
                front_node = node->r;
                back_node = node->l;
                split_voxel(v, node->plane, node->coord, &back_voxel, &front_voxel);
            }
            break;
            
        case YZ:
            if((node->coord.x - v.x_min) * (node->coord.x - vector_start.x) > 0) {
                front_node = node->l;
                back_node = node->r;
                split_voxel(v, node->plane, node->coord, &front_voxel, &back_voxel);
            } else {
                front_node = node->r;
                back_node = node->l;
                split_voxel(v, node->plane, node->coord, &back_voxel, &front_voxel);
            }
            break;
    }    

    
    Float t_near;
    Float t_far;
    
    if(voxel_intersection(vector, vector_start, front_voxel, &t_near, &t_far)
       && ((t_near >= 0) || (t_far >= 0))
       && find_intersection_node(front_node,
                                 front_voxel,
                                 vector_start,
                                 vector,
                                 nearest_obj_ptr,
                                 nearest_intersection_point_ptr,
                                 nearest_intersection_point_dist_ptr)) return True;
            
    return (voxel_intersection(vector, vector_start, back_voxel, &t_near, &t_far)
            && ((t_near >= 0) || (t_far >= 0))
            && find_intersection_node(back_node,
                                      back_voxel,
                                      vector_start,
                                      vector,
                                      nearest_obj_ptr,
                                      nearest_intersection_point_ptr,
                                      nearest_intersection_point_dist_ptr));
}

Boolean
is_intersect_anything_tree(KDTree * const tree,
                           const Point3d vector_start,
                           const Vector3d vector) {

    Float t_near;
    Float t_far;
    
    return (voxel_intersection(vector, vector_start, tree->bounding_box, &t_near, &t_far)
            && ((t_near >= 0) || (t_far >= 0))
            && is_intersect_anything_node(tree->root,
                                          tree->bounding_box,
                                          vector_start,
                                          vector));
}

inline Boolean
is_intersect_anything_node(KDNode * const node,
                           const Voxel v,
                           const Point3d vector_start,
                           const Vector3d vector) {
    
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
                    
                    if(obj->intersect(obj->data, vector_start, vector, &intersection_point)) {
                        
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
    
    Float t_near;
    Float t_far;
        
    if(voxel_intersection(vector, vector_start, v_l, &t_near, &t_far)
       && ((t_near >= 0) || (t_far >= 0))
       && is_intersect_anything_node(node->l, v_l, vector_start, vector)) return True;
    
    return (voxel_intersection(vector, vector_start, v_r, &t_near, &t_far)
            && ((t_near >= 0) || (t_far >= 0))
            && is_intersect_anything_node(node->r, v_r, vector_start, vector));
}
