#include <canvas.h>
#include <render.h>

#include "scene.h"

#define CANVAS_W 400
#define CANVAS_H 350

#define THREADS_NUM 4

int main(void) {
    Scene * scene = makeScene();
    
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    
    Point3d camera_location = point3d(-200, 170, 0);
    Float focus = 320;
    Float x_angle = -1.8;
    Float y_angle = 0;
    Float z_angle = 3.14;
    Camera * camera = new_camera(camera_location,
                                 x_angle,
                                 y_angle,
                                 z_angle,
                                 focus);

    /* Rotate camera if needed */
    // rotate_camera(camera, 0, 0, -0.08);
    
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
