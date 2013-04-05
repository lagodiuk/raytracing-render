#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "canvas.h"
#include "render.h"

#include "scene1.h"

GLint win_width = 512;
GLint win_height = 512;

#define tex_width  256
#define tex_height 256

typedef struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_t;

float delta_al = M_PI / 6;
Point3d camera_point = { X_CAM, Y_CAM, Z_CAM };
Scene *scene = NULL;

GLuint tex;
pixel_t canvas[tex_width][tex_height] = { 0 };

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
     glTexCoord2f(0.0, 0.0);
     glVertex2f( 1.0,  1.0);
     glTexCoord2f(1.0, 0.0);
     glVertex2f(-1.0,  1.0);
     glTexCoord2f(1.0, 1.0);
     glVertex2f(-1.0, -1.0);
     glTexCoord2f(0.0, 1.0);
     glVertex2f( 1.0, -1.0);
    glEnd();
    glutSwapBuffers();
    glDisable(GL_TEXTURE_2D);
    glFlush();
    //glutSwapBuffers();
}

void reshape(GLint w, GLint h) {
    win_width = w;
    win_height = h;
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

static inline uint8_t toGLubyte(GLfloat clampf) {
    return (uint8_t)(256 * clampf - 1);
}

void prepate_canvas(Scene* scene, pixel_t canvas[tex_width][tex_height]) {
    pixel_t current = { 0 };
    GLint i, j;
    for (i = 0; i < tex_width; ++i) 
        for (j = 0; j < tex_height; ++j) {
            Vector3d ray = { i - tex_width / 2, j - tex_height / 2, PROJ_PLANE_Z };
            trace(scene, camera_point, ray, (Color *)&current);
            canvas[j][i] = current;
        }
}

void animate() {
    delta_al += 0.05;
    rotate_scene(scene, delta_al, M_PI * 3 / 5, ROTATE_LIGHT_SOURCES);
    prepate_canvas(scene, canvas);
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, canvas);
    glDisable(GL_TEXTURE_2D);
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(win_width, win_height);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow("Raytracing");
    
    glutDisplayFunc(display);
    glutIdleFunc(animate);
   
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glViewport(0, 0, win_width, win_height);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    scene = makeScene();
    rotate_scene(scene, delta_al, M_PI * 3 / 5, ROTATE_LIGHT_SOURCES);
    prepate_canvas(scene, canvas);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, canvas);

    glutMainLoop();
}
