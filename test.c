#include <stdio.h>
#include <math.h>
#include <canvas.h>
#include <render.h>

#define PICTURES 60

#define PROJ_PLANE_Z 50

#define X_CAM -0
#define Y_CAM -0
#define Z_CAM -240

#define MIN_X -100
#define MAX_X 100

#define MIN_Y -100
#define MAX_Y 100

#define CANVAS_W (MAX_X - MIN_X)
#define CANVAS_H (MAX_Y - MIN_Y)

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define ROTATE_LIGHT_SOURCES True

void add_cube(Scene * scene, Point3d base, Float a, Material material);

void add_serpinsky_pyramid(Scene * scene, int depth,
                           Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                           Material material, Color color);

void add_pyramid(Scene * scene,
                 Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                 Material material, Color color);

int main() {
    Scene * scene = new_scene(500, 20, BACKGROUND_COLOR);
    
    add_light_source(scene, light_source_3d(point3d(-90, 90, 90), rgb(255, 255, 255)));
    add_light_source(scene, light_source_3d(point3d(90, -90, 90), rgb(255, 255, 0)));
    
    add_cube(scene, point3d(80, 80, -50), 100, material(1, 5, 0, 0, 0, 0));
    
    Float pyramid_edge = 150;
    add_serpinsky_pyramid(scene, 2,
                          point3d(-pyramid_edge/2, -pyramid_edge * 0.87 / 2, 0),
                          point3d(pyramid_edge/2, -pyramid_edge * 0.87 / 2, 0),
                          point3d(0, pyramid_edge * 0.87 / 2, 0),
                          point3d(0, 0, pyramid_edge * 0.87),
                          material(1, 5, 0, 0, 0, 0), rgb(200, 230, 40));
    
    add_object(scene, new_triangle(
                                     point3d(-300, -300, -80),
                                     point3d(300, -300, -80),
                                     point3d(300, 300, -80),
                                     rgb(55, 255, 55),
                                     material(1, 5, 5, 10, 0, 10)));
    add_object(scene, new_triangle(
                                      point3d(-300, -300, -80),
                                      point3d(-300, 300, -80),
                                      point3d(300, 300, -80),
                                      rgb(55, 255, 55),
                                      material(1, 5, 5, 10, 0, 10)));
    
    int i;
    int j;
    Color col;
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    Point3d camera_point = point3d(X_CAM, Y_CAM, Z_CAM);
    
    int k;
    float delta_al = 2 * M_PI / PICTURES;
    char filename[30];
    
    for(k = 1; k <= PICTURES; k++) {
        rotate_scene(scene, k * delta_al, M_PI * 3 / 5, ROTATE_LIGHT_SOURCES);
        
        for(i = MIN_X; i <= MAX_X; i++) {
            for(j = MIN_Y; j <= MAX_Y; j++) {
                trace(scene, camera_point, vector3df(i, j, PROJ_PLANE_Z), &col);
                
                set_pixel(i + CANVAS_W / 2, j + CANVAS_H / 2, col, canv);
            }
        }
        
        sprintf(filename, "out_%03d.bmp", k);
        write_bmp(filename, canv);
        clear_canvas(canv);
    }
    
    release(canv);
    release_scene(scene);
	return 0;
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