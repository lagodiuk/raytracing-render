CC = gcc

LIBPATH	 = -L./render/lib
INCLUDES = -I./render/include
LIBS = -lrender -lm

render_lib = ./render/lib/librender.a

CC_OPTS	 = -Wall
CC_OPTS += -O2

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CC_OPTS_OPEN_GL = -framework GLUT -framework OpenGL -DDARWIN
else
  CC_OPTS_OPEN_GL = -lglut -DPOSIX
endif
OPEN_GL_OPTS := $(CC_OPTS_OPEN_GL)


#
# Demo: thread pool
#

thread_pool_stress_test: thread_pool_stress_test.c $(render_lib)
	$(CC) thread_pool_stress_test.c $(INCLUDES) $(LIBPATH) $(LIBS) -o $@
	./thread_pool_stress_test


#
# Demo: ffmpeg video
#

test_video: test $(frame_dir)
	./test
	ffmpeg -qscale 2 -r 10 -b 10M  -i './out_%03d.bmp'  movie.mp4

$(frame_dir):
	mkdir -p $@

test: test.c scene1.h scene1.o $(render_lib)
	$(CC) $(CC_OPTS) $(INCLUDES) $(LIBPATH) $(LIBS) test.c scene1.o -o $@


#
# Demo: OpenGL frontend
#

THREADS_NUM = 8

rungl_2: test_gl_2
	./$< $(THREADS_NUM)

test_gl_2: $(render_lib) scene1.o scene1.h test_gl_2.c 
	$(CC) $(CC_OPTS) test_gl_2.c scene1.o $(OPEN_GL_OPTS) $(LIBPATH) $(INCLUDES) $(LIBS) -pthread -o $@


#
# Scene
#

scene1.o: scene1.c
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
	rm -f ./test ./thread_pool_stress_test ./test_gl_2;     \
	rm -f *.mp4;                          \
	rm -f *.bmp			\
