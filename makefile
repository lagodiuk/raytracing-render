UNAME := $(shell uname)

render_dir 	=	./render/lib
render_lib	=	$(render_dir)/librender.a

LIBPATH		=	$(addprefix -L, $(render_dir))
INCLUDES	=	-O2 $(addprefix -I, ./render/include)
LINKLIBS	= 	-lrender -lm -lpthread

frame_dir	=	./frames

#
# System specific OpenGL compilation options
#

ifeq ($(UNAME), Darwin)
CC_OPTS_TEST_GL = -framework GLUT -framework OpenGL -DDARWIN
else
CC_OPTS_TEST_GL = -lglut -DPOSIX
endif

LINK_OPTS	:=	$(CC_OPTS_TEST_GL)
CC_OPTS		:=	-Wall -O2
ifneq ($(NCPU),)
CC_OPTS		:=	$(CC_OPTS) -DN_WORKERS=$(NCPU)
endif

#
# Demo applications
#

thread_pool_stress_test: thread_pool_stress_test.c $(render_lib)
	gcc thread_pool_stress_test.c $(INCLUDES) $(LIBPATH) $(LINKLIBS) -o $@
	./thread_pool_stress_test

test_video: test $(frame_dir)
	cd $(frame_dir) && ../test
	ffmpeg -qscale 2 -r 10 -b 10M  -i '$(frame_dir)/out_%03d.bmp'  movie.mp4

test: test.c scene1.h scene1.o $(render_lib)
	gcc -O test.c scene1.o $(CC_OPTS) $(INCLUDES) $(LINK_OPTS) $(LIBPATH) $(LINKLIBS) -o $@

rungl: test_gl
	./$<

test_gl: $(render_lib) scene1.o mt_render.o mt_render.h scene1.h test_gl.c 
	gcc -pthread test_gl.c scene1.o mt_render.o $(CC_OPTS) $(INCLUDES) $(LINK_OPTS) $(LIBPATH) $(LINKLIBS) -o $@

$(frame_dir):
	mkdir -p $@

scene1.o: scene1.c
	gcc -c $< $(INCLUDES) $(CC_OPTS) -o $@

mt_render.o: mt_render.c
	gcc -c $< $(INCLUDES) $(CC_OPTS) -o $@

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
	rm -f ./test ./test_gl ./test_kd ./thread_pool_stress_test;     \
	rm -f *.mp4;                          \
	rm -rf $(frame_dir)			\
