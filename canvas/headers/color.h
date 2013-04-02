#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

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

inline Color rgb(Byte r, Byte g, Byte b);

inline Color add_colors(Color c1, Color c2);

inline Color mul_colors(Color c1, Color c2);

inline Color mul_color(Color c, float k);

#endif