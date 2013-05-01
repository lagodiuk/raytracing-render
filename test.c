#include <stdio.h>
#include <math.h>

#include <thread_pool.h>

#include "scene1.h"

#define PICTURES_COUNT 80

int main() {
    Scene *scene = makeScene();
    
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    
    Point3d camera_point = point3d(X_CAM, Y_CAM - 45, Z_CAM);
    
    float delta_al = 2 * M_PI / PICTURES_COUNT;
    char filename[30];

    int k;
    ThreadPool * thread_pool = new_thread_pool(4);
    
    for(k = 1; k <= PICTURES_COUNT; k++) {
        rotate_scene(scene,
                     k * delta_al,
                     M_PI * 3 / 5,
                     ROTATE_LIGHT_SOURCES);
        
        render_scene(scene,
                     camera_point,
                     PROJ_PLANE_Z,
                     canv,
                     thread_pool);
        
        sprintf(filename, "out_%03d.bmp", k);
        write_bmp(filename, canv);
        clear_canvas(canv);
    }
    
    release(canv);
    release_scene(scene);
	return 0;
}

