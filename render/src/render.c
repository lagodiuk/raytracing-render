#include <stdlib.h>

#include <render.h>
#include <canvas.h>
#include <color.h>
#include <thread_pool.h>

typedef
struct {
    Scene * scene;
    Point3d camera_position;
    Float proj_plane_dist;
    Canvas * canvas;
    
    Float dx;
    Float dy;
    
    int x_min;
    int x_max;
    int y_min;
    int y_max;
}
RenderSceneData;

void
render_part_of_scene_task(void * arg);

void
render_part_of_scene(RenderSceneData * data);

RenderSceneData *
new_render_scene_data(const Scene * const scene,
                      const Point3d camera_position,
                      const Float proj_plane_dist,
                      const Canvas * const canvas);

void
render_scene(Scene * scene,
             Point3d camera_position,
             Float proj_plane_dist,
             Canvas * canvas,
             ThreadPool * thread_pool) {
    
    RenderSceneData * data =
        new_render_scene_data(scene, camera_position, proj_plane_dist, canvas);
    
    render_part_of_scene(data);
    
    free(data);
}

RenderSceneData *
new_render_scene_data(const Scene * const scene,
                      const Point3d camera_position,
                      const Float proj_plane_dist,
                      const Canvas * const canvas) {
    
    RenderSceneData * data = malloc(sizeof(RenderSceneData));
    data->scene = scene;
    data->camera_position = camera_position;
    data->proj_plane_dist = proj_plane_dist;
    data->canvas = canvas;
    data->dx = canvas->w / 2.0;
    data->dy = canvas->h / 2.0;
    data->x_min = 0;
    data->x_max = canvas->w;
    data->y_min = 0;
    data->y_max = canvas->h;
    
    return data;
}

void
render_part_of_scene(RenderSceneData * data) {
    
    Float dx = data->dx;
    Float dy = data->dy;
    
    Scene * scene = data->scene;
    Point3d camera_position = data->camera_position;
    Float proj_plane_dist = data->proj_plane_dist;
    Canvas * canvas = data->canvas;
    
    int x_min = data->x_min;
    int x_max = data->x_max;
    int y_min = data->y_min;
    int y_max = data->y_max;
    
    int i;
    int j;
    
    Float x;
    Float y;
    
    Color col;
    
    for(i = x_min; i < x_max; i++) {
        for(j = y_min; j < y_max; j++) {
            
            x = i - dx;
            y = j - dy;
            
            trace(scene, camera_position, vector3df(x, y, proj_plane_dist), &col);
            
            set_pixel(i, j, col, canvas);
        }
    }
}

void
render_part_of_scene_task(void * arg) {
    RenderSceneData * data = (RenderSceneData *) arg;
    render_part_of_scene(data);
}