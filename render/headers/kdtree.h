#ifndef __KD_TREEE_H__
#define __KD_TREEE_H__

#include <render.h>

KDTree * build_kd_tree(Object3d ** objects, int objects_count);

void release_kd_tree(KDTree * tree);

void split_voxel(Voxel v,
                 enum Plane p,
                 Coord c,
                 Voxel * vl,
                 Voxel * vr);

int object_in_voxel(Object3d * obj, Voxel v);

int vector_plane_intersection(Vector3d vector,
                              Point3d vector_start,
                              enum Plane plane,
                              Coord coord,
                              Point3d * result,
                              Float * t);

int voxel_intersection(Vector3d vector,
                       Point3d vector_start,
                       Voxel v,
                       Float * t_near,
                       Float * t_far);

int find_intersection_tree(KDTree * tree,
                           Point3d vector_start,
                           Vector3d vector,
                           Object3d ** nearest_obj_ptr,
                           Point3d * nearest_intersection_point_ptr,
                           Float * nearest_intersection_point_dist_ptr);

#endif