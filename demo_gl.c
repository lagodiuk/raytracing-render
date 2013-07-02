#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#include <omp.h>
/* collapse is a feature from OpenMP 3 (2008) */
#if _OPENMP < 200805
    #define collapse(x)
#endif
#define CHUNK 50

#include "scene.h"

#define DX 10
#define DY 10
#define DZ 10
#define D_FOCUS 5
#define D_ANGLE 0.05

#define TEX_WIDTH  256
#define TEX_HEIGHT 256

GLint win_width = 512;
GLint win_height = 512;


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_t;


Scene * scene = NULL;

Camera * camera = NULL;

Boolean camera_state_changed = False;

int threads_num = 0;

Canvas * canv = NULL;

GLuint tex;
pixel_t canvas[TEX_WIDTH][TEX_HEIGHT];

// Needed for FPS counting
#define FPS_INTERVAL    10
int frames = 0;
struct timeval last_time;


void
processControls(int key,
                int x,
                int y);

void
processExit(unsigned char key,
            int x,
            int y);

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
                     threads_num);
        
        /* Copying rendered image from Canvas * canv to pixel_t canvas[TEX_WIDTH][TEX_HEIGHT]*/
        omp_set_num_threads((threads_num < 2) ? 1 : threads_num);
        #pragma omp parallel private(i, j, px, c)
        #pragma omp for collapse(2) schedule(dynamic, CHUNK)
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

    fps_handler();
}

void animate() {
    render_seq();

    glEnable(GL_TEXTURE_2D);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, canvas);

    glDisable(GL_TEXTURE_2D);

    glutPostRedisplay();
}

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);
    glutInitWindowSize(win_width, win_height);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow("Raytracing");
    
    glutKeyboardFunc(processExit);
    glutSpecialFunc(processControls);

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
    
    camera = new_camera(point3d(0, 100, 0), -M_PI / 2, 0, M_PI, 200);
    camera_state_changed = True;
    
    canv = new_canvas(TEX_WIDTH, TEX_HEIGHT);
    
    if(argc > 1) {
        threads_num = atoi(argv[1]);
    }

    render_seq();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, canvas);
        
    glutMainLoop();

    return 0;
}

void
processControls(int key,
                   int x,
                   int y) {
    
    int modifiers = glutGetModifiers();
    switch(key) {
            
		case GLUT_KEY_UP :
            switch(modifiers) {
                case GLUT_ACTIVE_CTRL :
                    move_camera(camera, vector3df(0, 0, DZ));
                    break;
                case GLUT_ACTIVE_ALT :
                    camera->proj_plane_dist += D_FOCUS;
                    break;
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(0, -DY, 0));
                    break;
                default :
                    rotate_camera(camera, D_ANGLE, 0, 0);
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
                    camera->proj_plane_dist -= D_FOCUS;
                    break;
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(0, DY, 0));
                    break;
                default :
                    rotate_camera(camera, -D_ANGLE, 0, 0);
                    break;
            }
            camera_state_changed = True;
            break;
            
        case GLUT_KEY_LEFT :
            switch(modifiers) {
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(DX, 0, 0));
                    break;
                case GLUT_ACTIVE_ALT :
                    rotate_camera(camera, 0, -D_ANGLE, 0);
                    break;
                default :
                    rotate_camera(camera, 0, 0, -D_ANGLE);
                    break;
            }
            camera_state_changed = True;
            break;
            
		case GLUT_KEY_RIGHT :
            switch(modifiers) {
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera, vector3df(-DX, 0, 0));
                    break;
                case GLUT_ACTIVE_ALT :
                    rotate_camera(camera, 0, D_ANGLE, 0);
                    break;
                default :
                    rotate_camera(camera, 0, 0, D_ANGLE);
                    break;
            }
            camera_state_changed = True;
            break;
	}
}

void
processExit(unsigned char key,
                  int x,
                  int y) {
    if (key == 27) {
        exit(0);
    }
}

/* Just simple dummy loop for clarifying absence of memory leaks in render */
void
dummy_test(int argc,
           char *argv[]) {
    
    scene = makeScene();
    
    camera = new_camera(point3d(0, 100, 0), -M_PI / 2, 0, M_PI, 200);
    
    canv = new_canvas(TEX_WIDTH, TEX_HEIGHT);
    
    if(argc > 1) {
        threads_num = atoi(argv[1]);
    }
    
    for(;;) {
        rotate_camera(camera, 0.05, 0, 0);
        render_scene(scene,
                     camera,
                     canv,
                     threads_num);
        fps_handler();
    }
}