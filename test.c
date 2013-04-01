#include <math.h>
#include <geometry3d.h>

int main() {
	Object3d * t = new_triangle(
                                point3d(0, 0, 0),
                                point3d(1, 0, 0),
                                point3d(0, 1, 0),
                                rgb(100, 255, 0));
    
	t->rotate(t->data, 0, M_PI / 3);
	t->print(t->data);
	return 0;
}
