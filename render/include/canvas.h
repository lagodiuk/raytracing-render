#ifndef __CANVAS_H__
#define __CANVAS_H__
#include "color.h"

#include <stdlib.h>
#include <string.h>

typedef
struct {
	int w;
	int h;
	Color * data;
}
Canvas;

Canvas *
new_canvas(int width,
           int height);

static inline void
clear_canvas(Canvas * canv) {
    memset(canv->data, 0, canv->w * canv->h * sizeof(Color));
}

static inline void
release(Canvas * c) {
	free(c->data);
	free(c);
}

static inline void
set_pixel(int x,
          int y,
          Color c,
          Canvas * canv) {
    
	int offs = y * canv->w + x;
	canv->data[offs] = c;
}

static inline Color
get_pixel(int x,
          int y,
          Canvas * canv) {
    
	int offs = y * canv->w + x;
	return canv->data[offs];
}

void draw_line(int x1,
               int y1,
               int x2,
               int y2,
               Color c,
               Canvas * canv);

int write_bmp(char file_name[],
              Canvas * canv);

#endif //__CANVAS_H__
