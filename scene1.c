#include <stdio.h>

#include <obj_loader.h>

#include "scene1.h"

#define MAX_POLYGONS_NUMBER 50000

#define SERPINSKY_PYRAMID_LEVEL 4

void
face_handler(Queue * vertexes,
             Queue * norm_vectors,
             void * arg);

Scene *makeScene(void) {
    Scene * scene = new_scene(MAX_POLYGONS_NUMBER, 5, BACKGROUND_COLOR);
    
    //add_light_source(scene, light_source_3d(point3d(-90, 90, 190), rgb(255, 255, 255)));
    //add_light_source(scene, light_source_3d(point3d(90, -90, 190), rgb(255, 255, 0)));
    add_light_source(scene, light_source_3d(point3d(X_CAM - 100, Y_CAM, Z_CAM), rgb(255, 255, 255)));

    set_exponential_fog(scene, 0.003);

    add_cube(scene, point3d(60, 60, -60), 90, material(3, 7, 0, 0, 0, 0));
    
    Float pyramid_edge = 200;
    Float dx = -100;
    Float dy = -40;
    
    add_serpinsky_pyramid(scene, SERPINSKY_PYRAMID_LEVEL,
                          point3d(-pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, dy, pyramid_edge * 0.87),
                          material(1, 5, 0, 2, 0, 0), rgb(240, 210, 40));
    
    add_object(scene, new_triangle(
                                     point3d(-300, -300, -80),
                                     point3d(300, -300, -80),
                                     point3d(300, 300, -80),
                                     rgb(55, 255, 55),
                                     material(1, 6, 0, 2, 0, 0)));
    add_object(scene, new_triangle(
                                      point3d(-300, -300, -80),
                                      point3d(-300, 300, -80),
                                      point3d(300, 300, -80),
                                      rgb(55, 255, 55),
                                      material(1, 6, 0, 2, 0, 0)));
    /*
    add_object(scene, new_sphere(point3d(130, -100, -30),
                                 50.0,
                                 rgb(250, 30, 30),
                                 material(1, 5, 5, 10, 0, 10)));
    */
    load_obj("ateneal.obj", face_handler, scene);
    
    prepare_scene(scene);
    
    printf("\nScene created\n");
    printf("\nNumber of polygons: %i\n", scene->last_object_index + 1);

    return scene;
}

void
face_handler(Queue * vertexes,
             Queue * norm_vectors,
             void * arg) {
    Scene * scene = (Scene *) arg;
    
    float scale = 0.05;
    Float dx = 30;
    Float dy = -100;
    Float dz = 20;
    
    Point3d * p1 = (Point3d *) get(vertexes);
    Point3d * p2 = (Point3d *) get(vertexes);
    Point3d * p3 = NULL;
    
    Vector3d * v1 = (Vector3d *) get(norm_vectors);
    Vector3d * v2 = (Vector3d *) get(norm_vectors);
    Vector3d * v3 = NULL;
    
    while(!is_empty(vertexes)) {
        p3 = (Point3d *) get(vertexes);
        v3 = (Vector3d *) get(norm_vectors);
        
        if(v1 && v2 && v3)
            add_object(scene, new_triangle_with_norms(
                                       point3d(p1->x * scale + dx, p1->y * scale + dy, p1->z * scale + dz),
                                       point3d(p2->x * scale + dx, p2->y * scale + dy, p2->z * scale + dz),
                                       point3d(p3->x * scale + dx, p3->y * scale + dy, p3->z * scale + dz),
                                       *v1,
                                       *v2,
                                       *v3,
                                       rgb(55, 55, 255),
                                       material(1, 3, 5, 0, 0, 10)));
        else
            add_object(scene, new_triangle(
                                       point3d(p1->x * scale + dx, p1->y * scale + dy, p1->z * scale + dz),
                                       point3d(p2->x * scale + dx, p2->y * scale + dy, p2->z * scale + dz),
                                       point3d(p3->x * scale + dx, p3->y * scale + dy, p3->z * scale + dz),
                                       rgb(55, 55, 255),
                                       material(1, 3, 5, 0, 0, 10)));
        
        p2 = p3;
        v2 = v3;
    }
}

void add_cube(Scene * scene, Point3d base, Float a, Material material) {
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x, base.y + a, base.z),
                                     rgb(255, 0, 0),
                                     material));
    add_object(scene, new_triangle(
                                     point3d(base.x + a, base.y + a, base.z),
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x, base.y + a, base.z),
                                     rgb(255, 0, 0),
                                     material));
    
    
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y, base.z + a),
                                     point3d(base.x + a, base.y, base.z + a),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 255, 0),
                                     material));
    add_object(scene, new_triangle(
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     point3d(base.x + a, base.y, base.z + a),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 255, 0),
                                     material));
    
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x, base.y + a, base.z),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 0, 255),
                                     material));
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x, base.y, base.z + a),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 0, 255),
                                     material));
    
    add_object(scene, new_triangle(
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x + a, base.y + a, base.z),
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     rgb(255, 255, 0),
                                     material));
    add_object(scene, new_triangle(
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x + a, base.y, base.z + a),
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     rgb(255, 255, 0),
                                     material));
    
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x, base.y, base.z + a),
                                     point3d(base.x + a, base.y, base.z),
                                     rgb(255, 0, 255),
                                     material));
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y, base.z + a),
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x + a, base.y, base.z + a),
                                     rgb(255, 0, 255),
                                     material));
    
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y + a, base.z),
                                     point3d(base.x, base.y + a, base.z + a),
                                     point3d(base.x + a, base.y + a, base.z),
                                     rgb(0, 255, 255),
                                     material));
    add_object(scene, new_triangle(
                                     point3d(base.x, base.y + a, base.z + a),
                                     point3d(base.x + a, base.y + a, base.z),
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     rgb(0, 255, 255),
                                     material));
}

void add_pyramid(Scene * scene, Point3d p1, Point3d p2, Point3d p3, Point3d p4, Material material, Color color) {
    add_object(scene, new_triangle(p1, p2, p3, color, material));
    add_object(scene, new_triangle(p1, p2, p4, color, material));
    add_object(scene, new_triangle(p2, p3, p4, color, material));
    add_object(scene, new_triangle(p3, p1, p4, color, material));
}

void add_serpinsky_pyramid(Scene * scene, int depth,
                           Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                           Material material, Color color) {
    if(depth) {
        Point3d p1p2 = point3d((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
        Point3d p1p3 = point3d((p1.x + p3.x) / 2, (p1.y + p3.y) / 2, (p1.z + p3.z) / 2);
        Point3d p1p4 = point3d((p1.x + p4.x) / 2, (p1.y + p4.y) / 2, (p1.z + p4.z) / 2);
        Point3d p2p3 = point3d((p2.x + p3.x) / 2, (p2.y + p3.y) / 2, (p2.z + p3.z) / 2);
        Point3d p2p4 = point3d((p2.x + p4.x) / 2, (p2.y + p4.y) / 2, (p2.z + p4.z) / 2);
        Point3d p3p4 = point3d((p3.x + p4.x) / 2, (p3.y + p4.y) / 2, (p3.z + p4.z) / 2);
        
        add_serpinsky_pyramid(scene, depth - 1, p1, p1p2, p1p3, p1p4, material, color);
        add_serpinsky_pyramid(scene, depth - 1, p2, p1p2, p2p3, p2p4, material, color);
        add_serpinsky_pyramid(scene, depth - 1, p3, p1p3, p2p3, p3p4, material, color);
        add_serpinsky_pyramid(scene, depth - 1, p4, p1p4, p2p4, p3p4, material, color);
    } else {
        add_pyramid(scene, p1, p2, p3, p4, material, color);
    }
}
