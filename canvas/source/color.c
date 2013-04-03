#include <math.h>
#include <color.h>

inline Color rgb(Byte r, Byte g, Byte b) {
        Color c;
        c.r = r;
        c.g = g;
        c.b = b;
        return c;
}

inline Color add_colors(Color c1, Color c2) {
	int r = (int) c1.r + c2.r;
	int g = (int) c1.g + c2.g;
	int b = (int) c1.b + c2.b;
	r = (r < MAX_R) ? r : MAX_R;
	g = (g < MAX_G) ? g : MAX_G;
	b = (b < MAX_B) ? b : MAX_B;
	return rgb((Byte) r, (Byte) g, (Byte) b);
}

/*
 * mul( rgb(125, 0, 125), rgb(0, 125, 0) ) == rgb (0, 0, 0)
 * mul( rgb(125, 125, 0), rgb(0, 125, 0) ) == rgb (0, 125, 0)
 * mul( rgb(0, 255, 0), rgb(0, 125, 0) ) == rgb (0, 178, 0)
 */
inline Color mul_colors(Color c1, Color c2) {
    int r = (int) sqrt((double) c1.r * c2.r);
	int g = (int) sqrt((double) c1.g * c2.g);
	int b = (int) sqrt((double) c1.b * c2.b);
    r = (r < MAX_R) ? r : MAX_R;
	g = (g < MAX_G) ? g : MAX_G;
	b = (b < MAX_B) ? b : MAX_B;
   	return rgb((Byte) r, (Byte) g, (Byte) b);
}

inline Color mul_color(Color c, float k) {
	return rgb((Byte) (c.r * k), (Byte) (c.g * k), (Byte) (c.b * k));
}
