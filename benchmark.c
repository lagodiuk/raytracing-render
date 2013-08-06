#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <canvas.h>
#include <render.h>
#include <obj_loader.h>

#define CANVAS_W 400
#define CANVAS_H 400

#define THREADS_NUM 1

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define MAX_OBJECTS_NUMBER 10000

#define MAX_LIGHT_SOURCES_NUMBER 5

#define MAX_SERPINSKY_PYRAMID_LEVEL 6

Camera *
create_camera(void);

void
create_serpinsky_pyramid(Scene * scene,
                         int level);

void add_serpinsky_pyramid(Scene * scene,
                           int depth,
                           Point3d p1,
                           Point3d p2,
                           Point3d p3,
                           Point3d p4,
                           Material material,
                           Color color);

void
add_pyramid(Scene * scene,
            Point3d p1,
            Point3d p2,
            Point3d p3,
            Point3d p4,
            Material material,
            Color color);

void
generate_random_spheres(Scene * scene,
                        int count);

int
main(void) {
    Camera * camera = create_camera();
    
    Canvas * canvas = new_canvas(CANVAS_W,
                                 CANVAS_H);
    
    int i;
    for(i = 0; i < MAX_SERPINSKY_PYRAMID_LEVEL; i++) {
        Scene * scene = new_scene(MAX_OBJECTS_NUMBER,
                                  MAX_LIGHT_SOURCES_NUMBER,
                                  BACKGROUND_COLOR);
    
        create_serpinsky_pyramid(scene, i);
        
        // Randomness causes unreproducible results
        // But in general - results are similar to Serpinsky pyramid
        //generate_random_spheres(scene, i * 400);

        prepare_scene(scene);
    
        printf("Number of polygons: %i. ", scene->last_object_index + 1);
    
        render_scene(scene,
                     camera,
                     canvas,
                     THREADS_NUM);
    
        release_scene(scene);
    }
    
    release_canvas(canvas);
    release_camera(camera);
    
    return 0;
}

Camera *
create_camera(void) {
    Point3d camera_location = point3d(-70, 200, 50);
    Float focus = 320;
    Float x_angle = -1.57;
    Float y_angle = 0;
    Float z_angle = 3.14;
    Camera * camera = new_camera(camera_location,
                                 x_angle,
                                 y_angle,
                                 z_angle,
                                 focus);
    return camera;
}

void
create_serpinsky_pyramid(Scene * scene,
                         int level) {
    Float pyramid_edge = 200;
    Float dx = -100;
    Float dy = -40;
    
    add_serpinsky_pyramid(scene,
                          level,
                          point3d(-pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(pyramid_edge/2 + dx, -pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, pyramid_edge * 0.87 / 2 + dy, 0),
                          point3d(dx, dy, pyramid_edge * 0.87),
                          material(1, 5, 0, 0, 0, 0), rgb(240, 210, 40));
}

void add_serpinsky_pyramid(Scene * scene,
                           int depth,
                           Point3d p1,
                           Point3d p2,
                           Point3d p3,
                           Point3d p4,
                           Material material,
                           Color color) {
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
add_pyramid(Scene * scene,
            Point3d p1,
            Point3d p2,
            Point3d p3,
            Point3d p4,
            Material material,
            Color color) {
    add_object(scene, new_triangle(p1, p2, p3, color, material));
    add_object(scene, new_triangle(p1, p2, p4, color, material));
    add_object(scene, new_triangle(p2, p3, p4, color, material));
    add_object(scene, new_triangle(p3, p1, p4, color, material));
}

void
generate_random_spheres(Scene * scene,
                        int count) {
    srand(time(NULL));
    int i;
    for(i = 0; i < count; i++) {
        int x = (rand() % 200) - (rand() % 200);
        int y = (rand() % 200) - (rand() % 200);
        int z = (rand() % 200) - (rand() % 200);
        int r = (rand() % 40);
        add_object(scene, new_sphere(point3d(x, y, z), r, rgb(255, 0, 0), material(1, 5, 0, 0, 0, 0)));
    }
}