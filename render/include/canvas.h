#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <color.h>

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

Canvas *
grayscale_canvas(Canvas * base,
                 int num_threads);

Canvas *
detect_edges_canvas(Canvas * base,
                    int num_threads);

void
release_canvas(Canvas * c);

void
clear_canvas(Canvas * canv);

static inline void
set_pixel(int x,
          int y,
          Color c,
          Canvas * canv) {
    
	const int offs = y * canv->w + x;
	canv->data[offs] = c;
}

static inline Color
get_pixel(int x,
          int y,
          Canvas * canv) {
    
	const int offs = y * canv->w + x;
	return canv->data[offs];
}

Canvas *
read_png(char * file_name);

void write_png(char file_name[],
               Canvas * canv);

#endif //__CANVAS_H__
