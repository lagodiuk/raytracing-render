raytracing-render
=================

Developing raytracing render from scratch.

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
Rendering scene and saving picture into file <i>simple_demo.png</i>
```bash
make simple_demo && ./simple_demo
```

### Demo with OpenGL front-end ###
All rendering routines are performing by this render, not OpenGL.
Just using GLUT to display rendered image.
```bash
make demo_gl && ./demo_gl 4
```
The argument <i>4</i> is not necessary. It used for defining number of OpenMP threads, for boosting render performance.
You can throw away this argument as well (it means rendering in 1 thread):
```bash
make demo_gl && ./demo_gl
```
* Use controls <b>← ↑ → ↓</b> to rotate camera
* Use <b>CTRL + ↑</b> or <b>CTRL + ↓</b> to move camera forward or backward
* Use <b>SHIFT + ↑</b> or <b>SHIFT + ↓</b> to move camera up or down
* Use <b>SHIFT + ←</b> or <b>SHIFT + →</b> to move camera left or right
* Use <b>ALT + ↑</b> or <b>ALT + ↓</b> to change focus of camera
* Use <b>ESC</b> to exit