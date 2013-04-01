#include <stdio.h>
#include <math.h>
#include <canvas.h>
#include <geometry3d.h>

void add_cube(Scene * scene, Point3d base, float a);

int main() {
    Scene * scene = new_scene(8);
    
    add_cube(scene, point3d(-5, -5, -5), 150);
    
    rotate_scene(scene, 0, M_PI / 3);
    
    int i;
    int j;
    Color col;
    
    Canvas * canv = new_canvas(200, 200);
    
    for(i = -100; i < 101; i++) {
        for(j = -100; j < 101; j++) {
            trace(scene, point3d(-20, -20, -140), vector3df(i, j, 50), &col);
            
            set_pixel(i+100, j+100, col, canv);
        }
    }
    
    write_bmp("out.bmp", canv);
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