#include <canvas.h>
#include <render.h>

#define CANVAS_W 400
#define CANVAS_H 400

#define THREADS_NUM 4

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define MAX_OBJECTS_NUMBER 10000
#define MAX_LIGHT_SOURCES_NUMBER 5

int main(void) {
    Scene * scene = new_scene(MAX_OBJECTS_NUMBER,
                              MAX_LIGHT_SOURCES_NUMBER,
                              BACKGROUND_COLOR);
    
    add_object(scene, new_sphere(point3d(0, 0, 0),
                                 100.0,
                                 rgb(250, 30, 30),
                                 material(1, 5, 5, 10, 0, 10)));
    
    prepare_scene(scene);
    
    add_light_source(scene, light_source_3d(point3d(-300, 300, 300), rgb(255, 255, 255)));
    
    Canvas * canv = new_canvas(CANVAS_W, CANVAS_H);
    
    Point3d camera_location = point3d(0, 500, 0);
    Float focus = 320;
    Float x_angle = -1.57;
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
    
	return 0;
}
