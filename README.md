raytracing-render
=================

Developing raytracing render from scratch.

### Key Features ###
* Using <b>[k-d tree](http://en.wikipedia.org/wiki/K-d_tree)</b> for fast traversal of 3D scene objects
* Using <b>[Surface Area Heuristic (SAH)](http://stackoverflow.com/a/4633332/653511)</b> for building optimal k-d tree
* Ability to render entire scene in parallel (using <b>OpenMP</b>)
* Ability to use textures (using <b>libpng</b> to load PNG images)
* Ability to save rendered image to file (using <b>libpng</b>)
* Two types of primitives: triangle and sphere
* Ability to load 3D models from <b>*.obj</b> format
* Phong shading
* Reflections, shadows, fog effect, multiple light sources