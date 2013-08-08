#include <stdio.h>
#include <math.h>

#include <obj_loader.h>
#include <canvas.h>

#include "scene.h"

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define MAX_POLYGONS_NUMBER 150000

#define MAX_LIGHT_SOURCES_NUMBER 5

#define SERPINSKY_PYRAMID_LEVEL 5

void add_cube(Scene * scene,
              Point3d base,
              Float a,
              Material material);

void add_serpinsky_pyramid(Scene * scene,
                           int depth,
                           Point3d p1,
                           Point3d p2,
                           Point3d p3,
                           Point3d p4,
                           Material material,
                           Color color);

void add_pyramid(Scene * scene,
                 Point3d p1,
                 Point3d p2,
                 Point3d p3,
                 Point3d p4,
                 Material material,
                 Color color);

void create_serpinsky_pyramid(Scene * scene);

void create_cube(Scene * scene);

void create_sphere(Scene * scene);

void create_floor_with_texture(Scene * scene);

void create_floor_without_texture(Scene * scene);

void load_lamp(Scene * scene);

void load_teapot(Scene * scene);

void load_man(Scene * scene);

void load_atenea(Scene * scene);

void load_venus(Scene * scene);

void load_elephant(Scene * scene);

void load_car(Scene * scene);

void load_minicooper(Scene * scene);

void
add_skybox(Scene * scene,
           Point3d base,
           Float a);

Scene * makeScene(void) {    
    Scene * scene = new_scene(MAX_POLYGONS_NUMBER, MAX_LIGHT_SOURCES_NUMBER, BACKGROUND_COLOR);
    
    add_light_source(scene, new_light_source(point3d(-300, 300, 300), rgb(255, 255, 255)));
    
    //set_exponential_fog(scene, 0.001);

    //create_cube(scene);
    
    //create_serpinsky_pyramid(scene);
    
    create_floor_with_texture(scene);
    
    //create_floor_without_texture(scene);
    
    create_sphere(scene);
    
    //load_lamp(scene);
    
    //load_teapot(scene);
    
    //load_man(scene);
    
    // 10k triangles
    load_atenea(scene);
    
    // 11k triangles
    load_venus(scene);
    
    // 10k triangles
    load_elephant(scene);
    
    // 97k triangles!
    //load_car(scene);
    
    // 79k triangles!
    //load_minicooper(scene);
    
    //add_skybox(scene, point3d(-2000, -2000, -2000), 4000);

    printf("\nNumber of polygons: %i\n", scene->last_object_index + 1);
    printf("\nBuilding Kd-Tree. Wait, please...\n");
    prepare_scene(scene);    
    printf("\nScene created\n");

    return scene;
}

void load_lamp(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  33, 30, -100, 30, 0, 0, 0,
                                  rgb(20, 250, 100),
                                  material(1, 3, 5, 0, 0, 10));
    load_obj("./models/lamp.obj",
             scene_face_handler,
             &load_params);
}

void load_teapot(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  25, 100, 100, 32, 0, 0, 0,
                                  rgb(250, 200, 50),
                                  material(1, 3, 4, 7, 0, 10));
    load_obj("./models/teapot.obj",
             scene_face_handler,
             &load_params);
}

void load_man(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  110, 100, -100, -80, 0, 0, 0,
                                  rgb(120, 120, 250),
                                  material(1, 5, 0, 0, 0, 10));
    load_obj("./models/man.obj",
             scene_face_handler,
             &load_params);
}

void load_atenea(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  0.05, -100, -100, -30, 0, 0, 0,
                                  rgb(250, 200, 50),
                                  //reflective surface
                                  material(2, 3, 7, 3, 0, 10));
                                  //material(4, 3, 7, 0, 0, 10));
    load_obj("./models/ateneal.obj",
             scene_face_handler,
             &load_params);
}

void load_venus(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  0.05, 100, -100, -80, 0, 0, 1.3,
                                  rgb(200, 200, 150),
                                  material(2, 3, 0, 0, 0, 0));
    load_obj("./models/venusl.obj",
             scene_face_handler,
             &load_params);
}

void load_elephant(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  0.3, -350, -150, -100, 0, 0, 0,
                                  rgb(50, 150, 250),
                                  material(2, 3, 0, 0, 0, 10));
    load_obj("./models/elephal.obj",
             scene_face_handler,
             &load_params);
}

void load_car(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  3, 200, -100, -100, M_PI / 4, M_PI / 2, 0,
                                  rgb(190, 190, 220),
                                  material(3, 3, 7, 5, 0, 10));
    load_obj("./models/car.obj",
             scene_face_handler,
             &load_params);
}

void load_minicooper(Scene * scene) {
    SceneFaceHandlerParams load_params =
    new_scene_face_handler_params(scene,
                                  3, -100, -350, -100, 0, M_PI / 2, 0,
                                  rgb(220, 220, 220),
                                  material(2, 3, 7, 5, 0, 10));
    load_obj("./models/minicooper.obj",
             scene_face_handler,
             &load_params);
}

void create_serpinsky_pyramid(Scene * scene) {
    Float pyramid_edge = 250;
    Float dx = 40;
    Float dy = 40;
    
    add_serpinsky_pyramid(scene, SERPINSKY_PYRAMID_LEVEL,
                          point3d(-pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, dy, pyramid_edge * 0.87),
                          material(1, 5, 0, 0, 0, 0), rgb(240, 210, 40));
}

void create_cube(Scene * scene) {
    add_cube(scene, point3d(60, 60, -60), 90, material(3, 7, 0, 0, 0, 0));
}

void create_sphere(Scene * scene) {
    add_object(scene, new_sphere(point3d(130, 100, -30),
                                 50.0,
                                 rgb(250, 30, 30),
                                 material(1, 5, 5, 10, 0, 10)));
}

void create_floor_with_texture(Scene * scene) {
    Canvas * tex = read_png("./models/wall.png");
    
    add_object(scene, new_triangle_with_texture(
                                                point3d(-300, -300, -120),
                                                point3d(300, -300, -120),
                                                point3d(300, 300, -120),
                                                point2d(5, 0),
                                                point2d(0, 0),
                                                point2d(0, 5),
                                                tex,
                                                rgb(55, 255, 55),
                                                material(1, 6, 0, 2, 0, 0)));
    add_object(scene, new_triangle_with_texture(
                                                point3d(-300, -300, -120),
                                                point3d(-300, 300, -120),
                                                point3d(300, 300, -120),
                                                point2d(5, 0),
                                                point2d(5, 5),
                                                point2d(0, 5),
                                                tex,
                                                rgb(55, 255, 55),
                                                material(1, 6, 0, 2, 0, 0)));
}

void create_floor_without_texture(Scene * scene) {   
    add_object(scene, new_triangle(
                                   point3d(-500, -500, -120),
                                   point3d(500, -500, -120),
                                   point3d(500, 500, -120),
                                   rgb(55, 200, 155),
                                   material(1, 6, 0, 2, 0, 0)));
    
    add_object(scene, new_triangle(
                                   point3d(-500, -500, -120),
                                   point3d(-500, 500, -120),
                                   point3d(500, 500, -120),
                                   rgb(55, 200, 155),
                                   material(1, 6, 0, 2, 0, 0)));
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




void
add_skybox(Scene * scene,
           Point3d base,
           Float a) {
    
    Material m = material(1, 0, 0, 0, 0, 0);
    
    
    Canvas * negz = read_png("./models/skybox/negy.png");
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y, base.z),
                                                point3d(base.x + a, base.y, base.z),
                                                point3d(base.x, base.y + a, base.z),
                                                point2d(0, 0),
                                                point2d(1, 0),
                                                point2d(0, 1),
                                                negz,
                                                rgb(255, 0, 0),
                                                m));
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x + a, base.y + a, base.z),
                                                point3d(base.x + a, base.y, base.z),
                                                point3d(base.x, base.y + a, base.z),
                                                point2d(1, 1),
                                                point2d(1, 0),
                                                point2d(0, 1),
                                                negz,
                                                rgb(255, 0, 0),
                                                m));
    
    
    Canvas * posz = read_png("./models/skybox/posy.png");
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y, base.z + a),
                                                point3d(base.x + a, base.y, base.z + a),
                                                point3d(base.x, base.y + a, base.z + a),
                                                point2d(0, 0),
                                                point2d(1, 0),
                                                point2d(0, -1),
                                                posz,
                                                rgb(0, 255, 0),
                                                m));
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x + a, base.y + a, base.z + a),
                                                point3d(base.x + a, base.y, base.z + a),
                                                point3d(base.x, base.y + a, base.z + a),
                                                point2d(1, -1),
                                                point2d(1, 0),
                                                point2d(0, -1),
                                                posz,
                                                rgb(0, 255, 0),
                                                m));
    
    
    Canvas * negx = read_png("./models/skybox/negx.png");
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y, base.z),
                                                point3d(base.x, base.y + a, base.z),
                                                point3d(base.x, base.y + a, base.z + a),
                                                point2d(0, 0),
                                                point2d(-1, 0),
                                                point2d(-1, -1),
                                                negx,
                                                rgb(0, 0, 255),
                                                m));
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y, base.z),
                                                point3d(base.x, base.y, base.z + a),
                                                point3d(base.x, base.y + a, base.z + a),
                                                point2d(0, 0),
                                                point2d(0, -1),
                                                point2d(-1, -1),
                                                negx,
                                                rgb(0, 0, 255),
                                                m));
    
    
    Canvas * posx = read_png("./models/skybox/posx.png");
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x + a, base.y, base.z),
                                                point3d(base.x + a, base.y + a, base.z),
                                                point3d(base.x + a, base.y + a, base.z + a),
                                                point2d(0, 0),
                                                point2d(1, 0),
                                                point2d(1, -1),
                                                posx,
                                                rgb(255, 255, 0),
                                                m));
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x + a, base.y, base.z),
                                                point3d(base.x + a, base.y, base.z + a),
                                                point3d(base.x + a, base.y + a, base.z + a),
                                                point2d(0, 0),
                                                point2d(0, -1),
                                                point2d(1, -1),
                                                posx,
                                                rgb(255, 255, 0),
                                                m));
    
    
    Canvas * negy = read_png("./models/skybox/posz.png");
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y, base.z),
                                                point3d(base.x, base.y, base.z + a),
                                                point3d(base.x + a, base.y, base.z),
                                                point2d(0, 0),
                                                point2d(0, -1),
                                                point2d(1, 0),
                                                negy,
                                                rgb(255, 0, 255),
                                                m));
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y, base.z + a),
                                                point3d(base.x + a, base.y, base.z),
                                                point3d(base.x + a, base.y, base.z + a),
                                                point2d(0, -1),
                                                point2d(1, 0),
                                                point2d(1, -1),
                                                negy,
                                                rgb(255, 0, 255),
                                                m));
    
    Canvas * posy = read_png("./models/skybox/negz.png");
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y + a, base.z),
                                                point3d(base.x, base.y + a, base.z + a),
                                                point3d(base.x + a, base.y + a, base.z),
                                                point2d(0, 0),
                                                point2d(0, -1),
                                                point2d(-1, 0),
                                                posy,
                                                rgb(0, 255, 255),
                                                m));
    add_object(scene, new_triangle_with_texture(
                                                point3d(base.x, base.y + a, base.z + a),
                                                point3d(base.x + a, base.y + a, base.z),
                                                point3d(base.x + a, base.y + a, base.z + a),
                                                point2d(0, -1),
                                                point2d(-1, 0),
                                                point2d(-1, -1),
                                                posy,
                                                rgb(0, 255, 255),
                                                m));
}