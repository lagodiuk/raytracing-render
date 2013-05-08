#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include <render.h>
#include <queue.h>

void
load_obj(const char * filename,
         void (* face_handler)(Queue * vertexes));

#endif
