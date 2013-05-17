#include <stdlib.h>
#include <string.h>

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

static inline void
render_part_of_scene_task(void * arg);

static inline void
render_part_of_scene(RenderSceneData * data);

static inline RenderSceneData *
new_render_scene_data(Scene * scene,
                      const Point3d camera_position,
                      const Float proj_plane_dist,
                      Canvas * canvas);

void
render_scene(Scene * scene,
             Point3d camera_position,
             Float proj_plane_dist,
             Canvas * canvas,
             ThreadPool * thread_pool) {
    
    if(!thread_pool) {
        RenderSceneData * data =
            new_render_scene_data(scene, camera_position, proj_plane_dist, canvas);
        
        render_part_of_scene(data);
        
        free(data);
        return;
    }
    
    int tasks_num = get_threads_num(thread_pool);
    Task ** tasks = calloc(tasks_num, sizeof(Task *));

    int slice_width = canvas->w / tasks_num;
    
    int i;
    RenderSceneData * data;
    
    for(i = 0; i < tasks_num; i++) {
        data = new_render_scene_data(scene, camera_position, proj_plane_dist, canvas);

        data->x_min = slice_width * i;
        data->x_max = slice_width * (i + 1);
        
        if(i == tasks_num - 1)
            data->x_max = canvas->w;
            
        tasks[i] = new_task(render_part_of_scene_task, data);
    }
    
    execute_tasks(tasks, tasks_num, thread_pool);
    wait_for_tasks(tasks, tasks_num);
    
    for(i = 0; i < tasks_num; i++) {
        free(tasks[i]->arg);
        destroy_task(tasks[i]);
    }
    free(tasks);
}

static inline RenderSceneData *
new_render_scene_data(Scene * scene,
                      const Point3d camera_position,
                      const Float proj_plane_dist,
                      Canvas * canvas) {
    
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

static inline void
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

static inline void
render_part_of_scene_task(void * arg) {
    RenderSceneData * data = (RenderSceneData *) arg;
    render_part_of_scene(data);
}