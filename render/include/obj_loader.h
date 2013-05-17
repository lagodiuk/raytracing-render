#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include <render.h>
#include <color.h>
#include <queue.h>

typedef
struct {
    Float scale;
    Float dx;
    Float dy;
    Float dz;
    Scene * scene;
    Color default_color;
    Material default_material;
}
SceneFaceHandlerParams;


void
load_obj(const char * filename,
         void (* face_handler)(Queue * vertexes,
                               Queue * norm_vectors,
                               void * args),
         void * args);

//----------------------------------------------------

void
scene_face_handler(Queue * vertexes,
                   Queue * norm_vectors,
                   void * arg);

static inline SceneFaceHandlerParams
new_scene_face_handler_params(Scene * scene,
                              Float scale,
                              Float dx,
                              Float dy,
                              Float dz,
                              Color default_color,
                              Material default_material) {
    
    SceneFaceHandlerParams params =
                    {.scene = scene,
                     .scale = scale,
                     .dx = dx,
                     .dy = dy,
                     .dz = dz,
                     .default_color = default_color,
                     .default_material = default_material};
    return  params;
}

#endif
