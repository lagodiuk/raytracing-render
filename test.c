#include <stdio.h>
#include <math.h>
#include <canvas.h>
#include <geometry3d.h>

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

#define BACKGROUND_COLOR rgb(0, 0, 0)

void add_cube(Scene * scene, Point3d base, float a);

int main() {
    Scene * scene = new_scene(8, BACKGROUND_COLOR);
    add_cube(scene, point3d(-5, -5, -5), 100);
    
    int i;
    int j;
    Color col;
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    Point3d camera_point = point3d(X_CAM, Y_CAM, Z_CAM);
    
    int k;
    float delta_al = 2 * M_PI / PICTURES;
    char filename[30];
    
    for(k = 1; k <= PICTURES; k++) {
        rotate_scene(scene, M_PI / 3 * 2.5, k * delta_al);
        
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

void add_cube(Scene * scene, Point3d base, float a) {
    scene->objects[0] = new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x, base.y + a, base.z),
                                     rgb(255, 0, 0));
    scene->objects[1] = new_triangle(
                                     point3d(base.x + a, base.y + a, base.z),
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x, base.y + a, base.z),
                                     rgb(255, 0, 0));
    
    
    scene->objects[2] = new_triangle(
                                     point3d(base.x, base.y, base.z + a),
                                     point3d(base.x + a, base.y, base.z + a),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 255, 0));
    scene->objects[3] = new_triangle(
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     point3d(base.x + a, base.y, base.z + a),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 255, 0));
    
    scene->objects[4] = new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x, base.y + a, base.z),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 0, 255));
    scene->objects[5] = new_triangle(
                                     point3d(base.x, base.y, base.z),
                                     point3d(base.x, base.y, base.z + a),
                                     point3d(base.x, base.y + a, base.z + a),
                                     rgb(0, 0, 255));
    
    scene->objects[6] = new_triangle(
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x + a, base.y + a, base.z),
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     rgb(255, 255, 0));
    scene->objects[7] = new_triangle(
                                     point3d(base.x + a, base.y, base.z),
                                     point3d(base.x + a, base.y, base.z + a),
                                     point3d(base.x + a, base.y + a, base.z + a),
                                     rgb(255, 255, 0));
}
