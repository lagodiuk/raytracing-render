#ifndef __KD_TREEE_H__
#define __KD_TREEE_H__

#include <render.h>

KDTree * build_kd_tree(Object3d ** objects, int objects_count);

void release_kd_tree(KDTree * tree);

int find_intersection_tree(KDTree * tree,
                           Point3d vector_start,
                           Vector3d vector,
                           Object3d ** nearest_obj_ptr,
                           Point3d * nearest_intersection_point_ptr,
                           Float * nearest_intersection_point_dist_ptr);

int is_intersect_anything_tree(KDTree * tree,
                               Point3d vector_start,
                               Vector3d vector);

#endif