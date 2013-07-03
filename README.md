raytracing-render
=================

Developing [ray tracing](http://en.wikipedia.org/wiki/Ray_tracing_%28graphics%29) render from scratch.

### Key Features ###
* Using [k-d tree](http://en.wikipedia.org/wiki/K-d_tree) for fast traversal of 3D scene objects
* Using [Surface Area Heuristic](http://stackoverflow.com/a/4633332/653511) for building optimal k-d tree
* Rendering entire scene in parallel (using [OpenMP](http://en.wikipedia.org/wiki/OpenMP))
* Texture mapping (using [libpng](http://en.wikipedia.org/wiki/Libpng))
* Saving rendered image to file (using [libpng](http://en.wikipedia.org/wiki/Libpng))
* Loading 3D models from [*.obj format](http://en.wikipedia.org/wiki/Wavefront_.obj_file)
* [Phong shading](http://en.wikipedia.org/wiki/Phong_shading)
* [Phong reflection model](http://en.wikipedia.org/wiki/Phong_reflection_model)
* Two types of primitives: triangle and sphere
* Reflections, shadows, fog effect, multiple light sources

### Requirements ###
Requires [libpng](http://www.libpng.org/pub/png/) to be installed.
Tested on Mac OS 10.8 with gcc 4.2 and gcc 4.7.

### Simple Demo ###
Rendering scene and saving picture to file <i>./demo/simple_demo.png</i>
```bash
(cd ./demo/ && make simple_demo && ./simple_demo)
```

### Demo with OpenGL front-end ###
All rendering routines are performing by this render, not OpenGL.
Just using GLUT to display rendered image.
```bash
(cd ./demo/ && make demo_gl && ./demo_gl 4)
```
The argument <i>4</i> is not necessary. It used for defining number of OpenMP threads, for boosting render performance.
You can throw away this argument as well (it means rendering in 1 thread):
```bash
(cd ./demo/ && make demo_gl && ./demo_gl)
```
* Use controls <b>← ↑ → ↓</b> to rotate camera
* Use <b>CTRL + ↑</b> or <b>CTRL + ↓</b> to move camera forward or backward
* Use <b>SHIFT + ↑</b> or <b>SHIFT + ↓</b> to move camera up or down
* Use <b>SHIFT + ←</b> or <b>SHIFT + →</b> to move camera left or right
* Use <b>ALT + ↑</b> or <b>ALT + ↓</b> to change focus of camera
* Use <b>ESC</b> to exit

### Create demo from scratch ###

```c
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
```
Launch it
```bash
(cd render/ && make render_lib)
gcc -I./render/include -L./render/lib/ -lrender -lpng -fopenmp demo_example.c -o example 
./example
```