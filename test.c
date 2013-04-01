#include <stdio.h>
#include <geometry3d.h>

int main() {
    Scene * scene = new_scene(1);
    
    scene->objects[0] = new_triangle(
                                     point3d(0, 0, 0),
                                     point3d(10, 0, 0),
                                     point3d(0, 10, 0),
                                     rgb(100, 255, 0));
    
    rotate_scene(scene, 0, 0);
    
    Color col;
    trace(scene, point3d(1, 1, 1), vector3df(0, 0, -1), &col);
    
    printf("r = %i, g = %i, b = %i\n", col.r, col.g, col.b);
    
    release_scene(scene);
	return 0;
}
