#include <stdio.h>
#include <math.h>

#include <canvas.h>
#include <render.h>

#include "scene.h"

#define CANVAS_W 400
#define CANVAS_H 300

#define THREADS_NUM 4

int main() {
    Scene * scene = makeScene();
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    Camera * camera = new_camera(point3d(0, 100, 0),
                                 -M_PI / 2, 0, M_PI,
                                 200);
    
    rotate_camera(camera, 0, 0, -0.08);
    
    render_scene(scene,
                 camera,
                 canv,
                 THREADS_NUM);
    
    write_png("simple_demo.png", canv);

    release_canvas(canv);
    release_scene(scene);
    release_camera(camera);
    
	exit(0);
}
