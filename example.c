#include <canvas.h>
#include <render.h>

#define CANVAS_W 400
#define CANVAS_H 400

#define THREADS_NUM 4

#define BACKGROUND_COLOR rgb(255, 255, 255)

#define MAX_OBJECTS_NUMBER 10000
#define MAX_LIGHT_SOURCES_NUMBER 5

int
main(void) {
    // Allocating scene
    Scene * scene = new_scene(MAX_OBJECTS_NUMBER,
                              MAX_LIGHT_SOURCES_NUMBER,
                              BACKGROUND_COLOR);
    
    // Allocating new sphere
    Float radius = 100;
    Point3d center = point3d(0, 0, 0);
    Color sphere_color = rgb(250, 30, 30);
    Material sphere_material = material(1, 5, 5, 10, 0, 10);
    Object3d * sphere = new_sphere(center,
                                   radius,
                                   sphere_color,
                                   sphere_material);
    
    // Adding sphere to the scene
    add_object(scene,
               sphere);
    
    // This function must be called after adding all objects to the scene
    // (initiates bulding of k-d tree of entire scene)
    prepare_scene(scene);
    
    // Allocating new light source
    Color light_source_color = rgb(255, 255, 255);
    Point3d light_source_location = point3d(-300, 300, 300);
    LightSource3d * light_source = new_light_source(light_source_location,
                                                    light_source_color);
    // Adding light source to the scene
    add_light_source(scene,
                     light_source);

    // Allocating camera
    // TODO: It's a pity, but quaternions are not implemented yet :(
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

    // Rotate camera if needed
    // rotate_camera(camera, d_x_angle, d_y_angle, d_z_angle);
    
    // Move camera if needed
    // move_camera(camera, vector3df(d_x, d_y, d_z));
    
    // Alocate new canvas, to render scene on it
    Canvas * canvas = new_canvas(CANVAS_W,
                                 CANVAS_H);
    
    render_scene(scene,
                 camera,
                 canvas,
                 THREADS_NUM);
    
    // Saving rendered image in PNG format
    write_png("example.png",
              canvas);

    release_canvas(canvas);
    release_scene(scene);
    release_camera(camera);
    
	return 0;
}
