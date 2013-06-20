#include <stdlib.h>
#include <string.h>

#include <render.h>
#include <canvas.h>
#include <color.h>

#include <omp.h>

void
render_scene(Scene * scene,
             Camera * const camera,
             Canvas * canvas,
             int num_threads) {
    
    const int w = canvas->w;
    const int h = canvas->h;
    const Float dx = w / 2.0;
    const Float dy = h / 2.0;
    const Float proj_plane_dist = camera->proj_plane_dist;
    
    const int iter = w * h;
    
    num_threads = (num_threads < 2) ? 1 : num_threads;
    omp_set_num_threads(num_threads);
    
    int i;
    #pragma omp parallel private(i)
    #pragma omp for schedule(dynamic)
    for(i = 0; i < iter; i++) {
        const canv_x = i % w;
        const canv_y = i / h;
        const Float x = canv_x - dx;
        const Float y = canv_y - dy;
        Color col;
        trace(scene, camera, vector3df(x, y, proj_plane_dist), &col);        
        set_pixel(canv_x, canv_y, col, canvas);
    }
}