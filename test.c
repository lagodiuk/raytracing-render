#include <stdio.h>
#include <math.h>

#define PICTURES 80

#include "scene1.h"

Point3d camera_point = { X_CAM, Y_CAM, Z_CAM };

int main() {
    Scene *scene = makeScene();

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
        
        for(i = MIN_X; i < MAX_X; i++) {
            for(j = MIN_Y; j < MAX_Y; j++) {
                trace(scene, camera_point, vector3df(i, j, PROJ_PLANE_Z), &col);
                
                set_pixel(i - MIN_X, j - MIN_Y, col, canv);
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

