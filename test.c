#include <stdio.h>
#include <math.h>
#include <canvas.h>
#include <geometry3d.h>

int main() {
    Scene * scene = new_scene(4);
    
    scene->objects[0] = new_triangle(
                                     point3d( 0,   0, 80),
                                     point3d(100,  0, 80),
                                     point3d( 0, 100, 80),
                                     rgb(255, 55, 0));
    
    scene->objects[1] = new_triangle(
                                     point3d(100, 100, 80),
                                     point3d(100,  0, 80),
                                     point3d( 0, 100, 80),
                                     rgb(255, 55, 0));
    
    scene->objects[2] = new_triangle(
                                     point3d( 0,   0, 80),
                                     point3d(100,  0, 80),
                                     point3d(100,  0, 180),
                                     rgb(55, 255, 0));
    
    scene->objects[3] = new_triangle(
                                     point3d( 0,   0, 80),
                                     point3d( 0,   0, 180),
                                     point3d(100,  0, 180),
                                     rgb(55, 255, 0));
    
    rotate_scene(scene, M_PI / 3, M_PI / 2);
    
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
