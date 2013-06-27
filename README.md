raytracing-render
=================

Developing raytracing render from scratch.

### Key Features ###
* Using [k-d tree](http://en.wikipedia.org/wiki/K-d_tree) for fast traversal of 3D scene objects
* Using [Surface Area Heuristic (SAH)](http://stackoverflow.com/a/4633332/653511) for building optimal k-d tree
* Rendering entire scene in parallel (using [OpenMP](http://en.wikipedia.org/wiki/OpenMP))
* Texture mapping (using [libpng](http://en.wikipedia.org/wiki/Libpng))
* Saving rendered image to file (using [libpng](http://en.wikipedia.org/wiki/Libpng))
* Two types of primitives: triangle and sphere
* Loading 3D models from [*.obj format](http://en.wikipedia.org/wiki/Wavefront_.obj_file)
* [Phong shading}(http://en.wikipedia.org/wiki/Phong_shading)
* Reflections, shadows, fog effect, multiple light sources

### Simple Demo ###
```bash
make simple_demo && ./simple_demo
```