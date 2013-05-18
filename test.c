#include <stdio.h>
#include <math.h>

#include <thread_pool.h>

#include "scene1.h"

#define CANVAS_W 200
#define CANVAS_H 200

#define PICTURES_COUNT 80

int main() {
    Scene * scene = makeScene();
    
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    
    Camera * camera = new_camera(point3d(0, 0, 0), M_PI, M_PI / 2, 200);
    
    ThreadPool * thread_pool = new_thread_pool(8);
    
    char filename[30];

    int k;
    
    for(k = 1; k <= PICTURES_COUNT; k++) {

        rotate_camera(camera, -0.08, 0);
        
        render_scene(scene,
                     camera,
                     canv,
                     thread_pool);
        
        sprintf(filename, "out_%03d.bmp", k);
        write_bmp(filename, canv);
        clear_canvas(canv);
        printf("%s\n", filename);
    }
    
    release(canv);
    release_scene(scene);
	return 0;
}

