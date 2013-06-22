#include <stdlib.h>
#include <string.h>

#include <render.h>
#include <canvas.h>
#include <color.h>

#include <omp.h>

#define CHUNK 10

/* collapse is a feature from OpenMP 3 (2008) */
#if _OPENMP < 200805
    #define collapse(x) 
#endif

void
render_scene(const Scene * const scene,
             const Camera * const camera,
             Canvas * canvas,
             const int num_threads) {
    
    const int w = canvas->w;
    const int h = canvas->h;
    const Float dx = w / 2.0;
    const Float dy = h / 2.0;
    const Float proj_plane_dist = camera->proj_plane_dist;
    
    // TODO: consider possibility to define these OpenMP parameters
    // in declarative style (using directives of preprocessor)
    //omp_set_dynamic(1);
    omp_set_num_threads((num_threads < 2) ? 1 : num_threads);
    
    int i;
    int j;
    #pragma omp parallel private(i, j)
    #pragma omp for collapse(2) schedule(dynamic, CHUNK)
    for(i = 0; i < w; i++) {
        for(j = 0; j < h; j++) {
            const Float x = i - dx;
            const Float y = j - dy;
            Color col;
            trace(scene, camera, vector3df(x, y, proj_plane_dist), &col);
            set_pixel(i, j, col, canvas);
        }
    }
}