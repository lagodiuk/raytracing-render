#include <render.h>
#include <kdtree.h>

#include <stdio.h>
#include <math.h>

#include "scene1.h"

void traverse(KDNode * node, Voxel v);

int main() {
    Scene * scene = makeScene();
    
    rotate_scene(scene, 0, M_PI * 3 / 5, True);
    
    KDTree * tree = build_kd_tree(scene->objects, scene->last_object_index + 1);
    
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    
    traverse(tree->root, tree->bounding_box);
    
    Object3d * obj = NULL;
    Point3d obj_point;
    Float obj_dist = -100;
    
    find_intersection_tree(tree, point3d(X_CAM, Y_CAM, Z_CAM), vector3df(0, 0, PROJ_PLANE_Z), &obj, &obj_point, &obj_dist);
    printf("\naaaaaaaaa %f\n", obj_dist);

    
    find_intersection(scene, point3d(X_CAM, Y_CAM, Z_CAM), vector3df(-10, -70, PROJ_PLANE_Z), &obj, &obj_point, &obj_dist);
    printf("\bbbbbbbb %f\n", obj_dist);
    
    release_kd_tree(tree);
    

    printf("\n");
    printf("\n");
    Point3d i;
    Float t;
    Coord c;
    c.z = 25;
    vector_plane_intersection(vector3df(0, 0, 1), point3d(7, 5, 0), XY, c, &i, &t);
    printf("xi: %f, yi: %f, zi: %f, t: %f\n", i.x, i.y, i.z, t);
    
    return 0;
}

void traverse(KDNode * node, Voxel v) {
    if(node->plane == NONE) {
        if(node->objects) {
            printf("\n");
            printf("count: %i\n", node->objects_count);
            printf("x_min = %f, y_min = %f, z_min = %f\n", v.x_min, v.y_min, v.z_min);
            printf("x_max = %f, y_max = %f, z_max = %f\n", v.x_max, v.y_max, v.z_max);
            
            int i;
            for(i = 0; i < node->objects_count; i++) {
                if(!object_in_voxel(node->objects[i], v)) {
                    printf("\nERROR!!!\n");
                    return;
                }
            }
        }
        return;
    }
    
    Voxel vl;
    Voxel vr;
    split_voxel(v, node->plane, node->coord, &vl, &vr);
    traverse(node->l, vl);
    traverse(node->r, vr);
}
