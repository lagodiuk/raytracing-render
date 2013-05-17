#ifndef __SCENE1_H__
#define __SCENE1_H__

#define PROJ_PLANE_Z 90

#define X_CAM -0
#define Y_CAM 40
#define Z_CAM -380

#define MIN_X -100
#define MAX_X 100

#define MIN_Y -100
#define MAX_Y 30

#define CANVAS_W (MAX_X - MIN_X)
#define CANVAS_H (MAX_Y - MIN_Y)

#define BACKGROUND_COLOR rgb(220, 240, 255)

#define ROTATE_LIGHT_SOURCES False

#include "canvas.h"
#include "render.h"

Scene *makeScene(void);

void add_cube(Scene * scene, Point3d base, Float a, Material material);

void add_serpinsky_pyramid(Scene * scene, int depth,
                           Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                           Material material, Color color);

void add_pyramid(Scene * scene,
                 Point3d p1, Point3d p2, Point3d p3, Point3d p4,
                 Material material, Color color);

#endif //__SCENE1_H__
