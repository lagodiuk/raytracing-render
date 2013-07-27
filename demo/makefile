CC = gcc

LIBPATH	 = -L../render/lib
INCLUDES = -I../render/include
LIBS = -lrender -lm -lpng -fopenmp

render = ../render/lib/librender.a

CC_OPTS	 = -Wall
CC_OPTS += -O2

ifeq ($(shell uname), Darwin)
  OPEN_GL_OPTS = -framework GLUT -framework OpenGL -DDARWIN
  # for libpng
  LIBPATH += -L/usr/X11/lib
  INCLUDES += -I/usr/X11/include
else
  OPEN_GL_OPTS = -lGL -lglut -DPOSIX
endif

THREADS_NUM = 4

run_demo_gl: demo_gl
	./$< $(THREADS_NUM)

demo_gl: $(render) scene.o scene.h demo_gl.c
	$(CC) $(CC_OPTS) -fopenmp demo_gl.c scene.o $(OPEN_GL_OPTS) $(LIBPATH) $(INCLUDES) $(LIBS) -o $@

scene.o: scene.c
	$(CC) $(INCLUDES) $(CC_OPTS) -c $< -o $@

$(render):
	(cd ../render && make DEF="$(DEF)" render)

.PHONY: clean
clean:
	(cd ../render && make clean) && \
	rm -f *.o;                  	  \
	rm -f ./demo_gl;		 
