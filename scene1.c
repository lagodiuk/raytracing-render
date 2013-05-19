#include <stdio.h>

#include <obj_loader.h>

#include "scene1.h"

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define MAX_POLYGONS_NUMBER 50000

#define MAX_LIGHT_SOURCES_NUMBER 5

#define SERPINSKY_PYRAMID_LEVEL 5

void add_cube(Scene * scene, Point3d base, Float a, Material material);

void add_serpinsky_pyramid(Scene * scene, int depth,
                           Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                           Material material, Color color);

void add_pyramid(Scene * scene,
                 Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                 Material material, Color color);

Scene *makeScene(void) {
    Scene * scene = new_scene(MAX_POLYGONS_NUMBER, MAX_LIGHT_SOURCES_NUMBER, BACKGROUND_COLOR);
    
    add_light_source(scene, light_source_3d(point3d(0, 0, 200), rgb(255, 255, 255)));

    set_exponential_fog(scene, 0.005);

    add_cube(scene, point3d(60, 60, -60), 90, material(3, 7, 0, 0, 0, 0));
    
    Float pyramid_edge = 200;
    Float dx = -100;
    Float dy = -40;
    
    add_serpinsky_pyramid(scene, SERPINSKY_PYRAMID_LEVEL,
                          point3d(-pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, dy, pyramid_edge * 0.87),
                          material(1, 5, 0, 0, 0, 0), rgb(240, 210, 40));
    
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
    
    add_object(scene, new_sphere(point3d(130, -100, -30),
                                 50.0,
                                 rgb(250, 30, 30),
                                 material(1, 5, 5, 10, 0, 10)));
    
    
    SceneFaceHandlerParams load_params;
    
    load_params =
        new_scene_face_handler_params(scene,
                                      33, 30, -100, 30,
                                      rgb(20, 250, 100),
                                      material(1, 3, 5, 0, 0, 10));
    load_obj("./models/lamp.obj",
             scene_face_handler,
             &load_params);
    
    load_params =
        new_scene_face_handler_params(scene,
                                      25, 100, 100, 32,
                                      rgb(250, 200, 50),
                                      material(1, 3, 0, 0, 0, 10));
    load_obj("./models/teapot.obj",
             scene_face_handler,
             &load_params);
    
    load_params =
    new_scene_face_handler_params(scene,
                                  130, -100, 100, -80,
                                  rgb(20, 20, 250),
                                  material(1, 5, 0, 0, 0, 10));
    load_obj("./models/man.obj",
             scene_face_handler,
             &load_params);

    printf("\nNumber of polygons: %i\n", scene->last_object_index + 1);
    
    prepare_scene(scene);
    
    printf("\nScene created\n");

    return scene;
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
