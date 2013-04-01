#include <stdio.h>
#include <math.h>
#include <canvas.h>
#include <geometry3d.h>

int main() {
    Scene * scene = new_scene(3);
    
    scene->objects[0] = new_triangle(
                                     point3d( 0,  0, 0),
                                     point3d(10,  0, 0),
                                     point3d( 0, 10, 0),
                                     rgb(100, 255, 0));
    
    scene->objects[1] = new_triangle(
                                     point3d( 0,  0, 20),
                                     point3d(10,  0, 20),
                                     point3d( 0, 10, 20),
                                     rgb(0, 255, 255));
    
    scene->objects[2] = new_triangle(
                                     point3d( 0,  0, 10),
                                     point3d(10,  0, 10),
                                     point3d( 0, 10, 10),
                                     rgb(0, 255, 0));
    
    rotate_scene(scene, M_PI / 4, M_PI / 2);
    
    int i;
    int j;
    Color col;
    
    Canvas * canv = new_canvas(200, 200);
    
    for(i = -100; i < 101; i++) {
        for(j = -100; j < 101; j++) {
            trace(scene, point3d(0, 0, -10), vector3df(i, j, -5), &col);
            
            set_pixel(i+100, j+100, col, canv);
        }
    }
    
    write_bmp("out.bmp", canv);
    release(canv);
    
    release_scene(scene);
	return 0;
}
