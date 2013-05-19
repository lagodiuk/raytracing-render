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

#include <canvas.h>
#include <render.h>

#include "scene1.h"

GLint win_width = 512;
GLint win_height = 512;

#define TEX_WIDTH  256
#define TEX_HEIGHT 256

#define DX 10
#define DY 10
#define DZ 10


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_t;


Scene * scene = NULL;

Camera * camera = NULL;

Boolean camera_state_changed = False;

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

static int render_seq(void) {
    pixel_t px;
    GLint i, j;
    Color c;
    
    if(camera_state_changed) {
        render_scene(scene,
                     camera,
                     canv,
                     thread_pool);
        
        for (j = 0; j < TEX_HEIGHT; ++j) {
            for (i = 0; i < TEX_WIDTH; ++i) {
                c = get_pixel(i, j, canv);
                memcpy(&px, &c, sizeof(pixel_t));
                canvas[j][i] = px;
            }
        }
        
        camera_state_changed = False;
    }    
    
    return 0;
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
    render_seq();

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
    int modifiers = glutGetModifiers();
    switch(key) {
            
		case GLUT_KEY_UP :
            switch(modifiers) {
                case GLUT_ACTIVE_CTRL :
                    move_camera(camera, vector3df(0, 0, DZ));
                    break;
                case GLUT_ACTIVE_ALT :
                    camera->proj_plane_dist += 5;
                    break;
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(0, -DY, 0));
                    break;
                default :
                    rotate_camera(camera, 0.0, -0.05);
                    break;                    
            }
            camera_state_changed = True;
            break;
            
		case GLUT_KEY_DOWN :
            switch(modifiers) {
                case GLUT_ACTIVE_CTRL :
                    move_camera(camera, vector3df(0, 0, -DZ));
                    break;
                case GLUT_ACTIVE_ALT :
                    camera->proj_plane_dist -= 5;
                    break;
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(0, DY, 0));
                    break;
                default :
                    rotate_camera(camera, 0.0, 0.05);
                    break;
            }
            camera_state_changed = True;
            break;
            
        case GLUT_KEY_LEFT :
            switch(modifiers) {
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(DX, 0, 0));
                    break;
                default :
                    rotate_camera(camera, 0.05, 0);
                    break;
            }
            camera_state_changed = True;
            break;
            
		case GLUT_KEY_RIGHT :
            switch(modifiers) {
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(-DX, 0, 0));
                    break;
                default :
                    rotate_camera(camera, -0.05, 0);
                    break;
            }
            camera_state_changed = True;
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
    
    camera = new_camera(point3d(0, 0, 0), M_PI, M_PI / 2, 200);
    camera_state_changed = True;
    
    canv = new_canvas(TEX_WIDTH, TEX_HEIGHT);
    
    if(argc > 1) {
        thread_pool = new_thread_pool(atoi(argv[1]));
    } else {
        thread_pool = NULL;
    }

    render_seq();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, canvas);
        
    glutMainLoop();
    
    /*
     
     scene = makeScene();
     
     canv = new_canvas(TEX_WIDTH, TEX_HEIGHT);
     
     if(argc > 1) {
        thread_pool = new_thread_pool(atoi(argv[1]));
     } else {
        thread_pool = NULL;
     }
     
     // Just simple dummy loop for clarifying absence of memory leaks in render 
     // (separated from OpenGL routines)
     
     for(;;) {
        rotate_camera(camera, 0.05, 0);
        render_scene(scene,
                     camera,
                     canv,
                     thread_pool);
        fps_handler();
     }
     */

    return 0;
}

