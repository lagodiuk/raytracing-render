CC = gcc

LIBPATH	 = -L./render/lib
INCLUDES = -I./render/include
LIBS = -lrender -lm -lpng -fopenmp

render_lib = ./render/lib/librender.a

CC_OPTS	 = -Wall
CC_OPTS += -O2

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CC_OPTS_OPEN_GL = -framework GLUT -framework OpenGL -DDARWIN
  # for libpng
  LIBPATH += -L/usr/X11/lib
  INCLUDES += -I/usr/X11/include
else
  CC_OPTS_OPEN_GL = -lGL -lglut -DPOSIX
endif
OPEN_GL_OPTS := $(CC_OPTS_OPEN_GL)

#
# Simple demo
#

simple_demo: simple_demo.c scene.h scene.o $(render_lib)
	$(CC) $(CC_OPTS) $(INCLUDES) $(LIBPATH) $(LIBS) simple_demo.c scene.o -o $@


#
# Demo: OpenGL frontend
#

THREADS_NUM = 8

run_demo_gl: demo_gl
	./$< $(THREADS_NUM)

demo_gl: $(render_lib) scene.o scene.h demo_gl.c
	$(CC) $(CC_OPTS) demo_gl.c scene.o $(OPEN_GL_OPTS) $(LIBPATH) $(INCLUDES) $(LIBS) -o $@


#
# Scene
#

scene.o: scene.c
	$(CC) $(INCLUDES) $(CC_OPTS) -c $< -o $@


#
# Render
#

$(render_lib):
	(cd ./render && make ./lib/librender.a)


#
# Routines
#

.PHONY: clean
clean:
	(cd ./render && make clean)	    &&\
	rm -f *.o;                            \
	rm -f ./simple_demo ./thread_pool_stress_test ./demo_gl;     \
	rm -f *.mp4;                          \
	rm -f *.bmp *.png			\
