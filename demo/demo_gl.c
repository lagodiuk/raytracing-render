/*
 *
 * This demo use GLUT as front-end to ray tracing render:
 * - displaying rendered scene in window
 * - handling keyboard controls for moving and rotating the camera
 *
 * TODO: maybe need some refactoring, as I am totally newbie to OpenGL and GLUT
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

Scene * scene = NULL;
Camera * camera = NULL;
Canvas * canv = NULL;

Boolean camera_state_changed = False;

int threads_num = 0;

typedef
struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}
pixel_t;

GLint win_width = 512;
GLint win_height = 512;

GLuint tex;
pixel_t canvas[TEX_WIDTH][TEX_HEIGHT];

void
processControls(int key,
                int x,
                int y);

void
processExit(unsigned char key,
            int x,
            int y);

void
init_scene_and_camera(void);

void
glut_routines(void);

void
display(void);

void
animate(void);

void
render_seq(void);

int
main(int argc,
     char *argv[]) {
    
    init_scene_and_camera();
    threads_num = (argc > 1) ? atoi(argv[1]) : 1;

    glutInit(&argc, argv);
    glut_routines();
    glutMainLoop();

    return 0;
}

void
init_scene_and_camera(void) {
    
    scene = makeScene();
    
    Float focus = 200;
    Float x_angle = -M_PI / 2;
    Float y_angle = 0;
    Float z_angle = M_PI;
    
    camera = new_camera(point3d(-80, 250, 50),
                        x_angle,
                        y_angle,
                        z_angle,
                        focus);
    
    camera_state_changed = True;
    
    canv = new_canvas(TEX_WIDTH,
                      TEX_HEIGHT);
}

void
glut_routines(void) {
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
    
    render_seq();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, canvas);
}

void
animate(void) {
    
    render_seq();
    
    glEnable(GL_TEXTURE_2D);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, canvas);
    glDisable(GL_TEXTURE_2D);
    glutPostRedisplay();
}

void
render_seq(void) {
    
    if(camera_state_changed) {
        render_scene(scene,
                     camera,
                     canv,
                     threads_num);
        
        pixel_t px;
        GLint i;
        GLint j;
        Color c;
        
        /* Copying rendered image from Canvas * canv to pixel_t canvas[TEX_WIDTH][TEX_HEIGHT]*/
        // TODO: memcpy entire arrays
        omp_set_num_threads(threads_num);
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
}

void
display(void) {
    
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
                    move_camera(camera,
                                vector3df(0, 0, DZ));
                    break;
                case GLUT_ACTIVE_ALT :
                    camera->proj_plane_dist += D_FOCUS;
                    break;
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera,
                                vector3df(0, -DY, 0));
                    break;
                default :
                    rotate_camera(camera,
                                  D_ANGLE, 0, 0);
                    break;
            }
            camera_state_changed = True;
            break;
            
		case GLUT_KEY_DOWN :
            switch(modifiers) {
                case GLUT_ACTIVE_CTRL :
                    move_camera(camera,
                                vector3df(0, 0, -DZ));
                    break;
                case GLUT_ACTIVE_ALT :
                    camera->proj_plane_dist -= D_FOCUS;
                    break;
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera,
                                vector3df(0, DY, 0));
                    break;
                default :
                    rotate_camera(camera,
                                  -D_ANGLE, 0, 0);
                    break;
            }
            camera_state_changed = True;
            break;
            
        case GLUT_KEY_LEFT :
            switch(modifiers) {
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera,
                                vector3df(DX, 0, 0));
                    break;
                case GLUT_ACTIVE_ALT :
                    rotate_camera(camera,
                                  0, -D_ANGLE, 0);
                    break;
                default :
                    rotate_camera(camera,
                                  0, 0, -D_ANGLE);
                    break;
            }
            camera_state_changed = True;
            break;
            
		case GLUT_KEY_RIGHT :
            switch(modifiers) {
                case GLUT_ACTIVE_SHIFT :
                    move_camera(camera,
                                vector3df(-DX, 0, 0));
                    break;
                case GLUT_ACTIVE_ALT :
                    rotate_camera(camera,
                                  0, D_ANGLE, 0);
                    break;
                default :
                    rotate_camera(camera,
                                  0, 0, D_ANGLE);
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