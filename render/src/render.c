#include <render.h>
#include <canvas.h>
#include <color.h>
#include <thread_pool.h>

void
render_scene(Scene * scene,
             Point3d camera_position,
             Float proj_plane_dist,
             Canvas * canvas,
             ThreadPool * thread_pool) {
    
    int i;
    int j;
    
    Float w_2 = canvas->w / 2;
    Float h_2 = canvas->h / 2;
    
    Float x;
    Float y;
    
    Color col;
    
    for(i = 0; i < canvas->w; i++) {
        for(j = 0; j < canvas->h; j++) {
            
            x = i - w_2;
            y = j - h_2;
            
            trace(scene, camera_position, vector3df(x, y, proj_plane_dist), &col);
            
            set_pixel(i, j, col, canvas);
        }
    }
}