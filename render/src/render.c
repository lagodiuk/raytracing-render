#include <stdlib.h>
#include <string.h>

#include <render.h>
#include <canvas.h>
#include <color.h>

#include <omp.h>

typedef
struct {
    Scene * scene;
    Camera * camera;
    Canvas * canvas;
    
    Float dx;
    Float dy;
    
    int x_min;
    int x_max;
    int y_min;
    int y_max;
}
RenderSceneData;

inline void
render_part_of_scene(const RenderSceneData * const data);

inline RenderSceneData *
new_render_scene_data(Scene * const scene,
                      Camera * const camera,
                      Canvas * const canvas);

void
render_scene(Scene * scene,
             Camera * const camera,
             Canvas * canvas,
             int num_threads) {
    
    if(num_threads < 2) {
        RenderSceneData * data =
            new_render_scene_data(scene, camera, canvas);
        
        render_part_of_scene(data);
        
        free(data);
        return;
    }

    int slice_width = canvas->w / num_threads;
    
    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        int i = omp_get_thread_num();
        
        RenderSceneData * data = new_render_scene_data(scene, camera, canvas);
        data->x_min = slice_width * i;
        data->x_max = slice_width * (i + 1);
        
        if(i == num_threads - 1)
            data->x_max = canvas->w;
        
        render_part_of_scene(data);
        
        free(data);
    }
}

inline RenderSceneData *
new_render_scene_data(Scene * const scene,
                      Camera * const camera,
                      Canvas * const canvas) {
    
    RenderSceneData * data = malloc(sizeof(RenderSceneData));
    data->scene = scene;
    data->camera = camera;
    data->canvas = canvas;
    data->dx = canvas->w / 2.0;
    data->dy = canvas->h / 2.0;
    data->x_min = 0;
    data->x_max = canvas->w;
    data->y_min = 0;
    data->y_max = canvas->h;
    
    return data;
}

inline void
render_part_of_scene(const RenderSceneData * const data) {
    
    const Float dx = data->dx;
    const Float dy = data->dy;
    
    Scene * const scene = data->scene;
    Camera * const camera = data->camera;
    Canvas * canvas = data->canvas;
    const Float proj_plane_dist = camera->proj_plane_dist;
    
    const int x_min = data->x_min;
    const int x_max = data->x_max;
    const int y_min = data->y_min;
    const int y_max = data->y_max;
    
    int i;
    int j;
    
    Float x;
    Float y;
    
    Color col;
    
    for(i = x_min; i < x_max; i++) {
        for(j = y_min; j < y_max; j++) {
            
            x = i - dx;
            y = j - dy;
            
            trace(scene, camera, vector3df(x, y, proj_plane_dist), &col);
            
            set_pixel(i, j, col, canvas);
        }
    }
}