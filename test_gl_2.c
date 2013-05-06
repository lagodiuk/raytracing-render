#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

// Mac OS X
#ifdef DARWIN
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

// The Rest of the World
#ifdef POSIX
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "canvas.h"
#include "render.h"

#include "scene1.h"

GLint win_width = 512;
GLint win_height = 512;

#define TEX_WIDTH  256
#define TEX_HEIGHT 256


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_t;

float al = M_PI / 6;
float be = M_PI * 3 / 5;

Point3d camera_point = { X_CAM, Y_CAM, Z_CAM };
Scene *scene = NULL;
ThreadPool * thread_pool = NULL;
Canvas * canv = NULL;

GLuint tex;
pixel_t canvas[TEX_WIDTH][TEX_HEIGHT];

// Needed for FPS counting
#define FPS_INTERVAL    10
int frames = 0;
struct timeval last_time;

void fps_handler(void) {
    ++frames;
    struct timeval this_time = { 0 };
    gettimeofday(&this_time, NULL);
    if (this_time.tv_sec >= last_time.tv_sec + FPS_INTERVAL) {
        float dt = this_time.tv_sec - last_time.tv_sec;
        last_time = this_time;
        float fps = frames / dt;
        printf("FPS: %f\n", fps);
        frames = 0;
    }
}

static int render_seq() {
    pixel_t px;
    GLint i, j;
    Color c;

    render_scene(scene,
                 camera_point,
                 PROJ_PLANE_Z,
                 canv,
                 thread_pool);
    
    for (j = 0; j < TEX_HEIGHT; ++j)
        for (i = 0; i < TEX_WIDTH; ++i) {
            c = get_pixel(i, j, canv);
            memcpy(&px, &c, sizeof(pixel_t));
            canvas[j][i] = px;
        }
    return 0;
}

void prepare_canvas(void) {
    render_seq();
}


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
    glDisable(GL_TEXTURE_2D);

    glFlush();

    GLenum glerr = glGetError();
    if (glerr) printf(__FILE__": glGetError() -> %d\n", glerr);

    fps_handler();
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


void animate() {
    al += 0.05;
    
    rotate_scene(scene, al, be, ROTATE_LIGHT_SOURCES);
    prepare_canvas();

    glEnable(GL_TEXTURE_2D);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, canvas);
    GLenum glerr = glGetError();
    if (glerr) printf("glGetError() -> %d\n", glerr);

    glDisable(GL_TEXTURE_2D);

    glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int x, int y) {
    if (key == 27)
		exit(0);
}

void processSpecialKeys(int key, int x, int y) {
    switch(key) {
		case GLUT_KEY_UP :
            be += 0.05;
            break;
		case GLUT_KEY_DOWN :
            be -= 0.05;
            break;
        case GLUT_KEY_LEFT :
            al += 0.05;
            break;
		case GLUT_KEY_RIGHT :
            al -= 0.05;
            break;
	}
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(win_width, win_height);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow("Raytracing");
    
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    glClearColor(0.0, 1.0, 0.0, 1.0);
    glViewport(0, 0, win_width, win_height);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glutDisplayFunc(display);
    glutIdleFunc(animate);

    scene = makeScene();
    canv = new_canvas(TEX_WIDTH, TEX_HEIGHT);
    
    if(argc > 1) {
        thread_pool = new_thread_pool(atoi(argv[1]));
    } else {
        thread_pool = NULL;
    }

    rotate_scene(scene, al, M_PI * 3 / 5, ROTATE_LIGHT_SOURCES);
    prepare_canvas();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, canvas);
        
    glutMainLoop();

    return 0;
}

