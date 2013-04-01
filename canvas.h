#include <color.h>

typedef
struct {
	int w;
	int h;
	Color * data;
}
Canvas;

Canvas * new_canvas(int width, int height);

void release(Canvas * canv);

inline void set_pixel(int x, int y, Color c, Canvas * canv);

inline Color get_pixel(int x, int y, Canvas * canv);

int write_bmp(char file_name[], Canvas * canv);
