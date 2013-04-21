#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <math.h>
#include <stdint.h>

#define MAX_R 255
#define MAX_G 255
#define MAX_B 255

typedef
uint8_t
Byte;

typedef
struct {
        Byte r;
        Byte g;
        Byte b;
}
Color;

static inline Color rgb(Byte r, Byte g, Byte b) {
        Color c;
        c.r = r;
        c.g = g;
        c.b = b;
        return c;
}

static inline Color add_colors(Color c1, Color c2) {
	int r = (int) c1.r + c2.r;
	int g = (int) c1.g + c2.g;
	int b = (int) c1.b + c2.b;
	r = (r < MAX_R) ? r : MAX_R;
	g = (g < MAX_G) ? g : MAX_G;
	b = (b < MAX_B) ? b : MAX_B;
	return rgb((Byte) r, (Byte) g, (Byte) b);
}

static inline Color mul_colors(Color c1, Color c2) {
    uint16_t r = (c1.r + c2.r) >> 1;
	uint16_t g = (c1.g + c2.g) >> 1;
	uint16_t b = (c1.b + c2.b) >> 1;

   	return rgb((Byte) r, (Byte) g, (Byte) b);
}

static inline Color mul_color(Color c, double k) {
	return rgb((Byte) (c.r * k), (Byte) (c.g * k), (Byte) (c.b * k));
}

#endif
