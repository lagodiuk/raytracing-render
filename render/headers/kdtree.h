#ifndef __KD_TREEE_H__
#define __KD_TREEE_H__

#include <render.h>

KDTree *
build_kd_tree(Object3d ** objects,
              int objects_count);

void
release_kd_tree(KDTree * tree);

Boolean
find_intersection_tree(KDTree * const tree,
                       const Point3d vector_start,
                       const Vector3d vector,
                       Object3d ** const nearest_obj_ptr,
                       Point3d * const nearest_intersection_point_ptr,
                       Float * const nearest_intersection_point_dist_ptr);

Boolean
is_intersect_anything_tree(KDTree * const tree,
                           const Point3d vector_start,
                           const Vector3d vector);

#endif