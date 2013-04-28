UNAME := $(shell uname)

render_dir 	=	./render/lib
render_lib	=	$(render_dir)/librender.a

LIBPATH		=	$(addprefix -L, $(render_dir))
INCLUDES	=	-O2 $(addprefix -I, ./render/include)
LINKLIBS	= 	-lrender -lm

frame_dir	=	./frames

#
# System specific OpenGL compilation options
#

ifeq ($(UNAME), Darwin)
CC_OPTS_TEST_GL = -framework GLUT -framework OpenGL -DDARWIN
# Generating by gcc in Mac OS
d_sym	= *.dSYM
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

$(render_dir):
	mkdir -p $@

$(render_dir)/canvas.o: ./render/src/canvas.c ./render/include/canvas.h ./render/include/color.h $(render_dir)
	gcc -c ./render/src/canvas.c $(INCLUDES) -o $@

$(render_dir)/scene.o: ./render/src/scene.c ./render/include/render.h ./render/include/color.h $(render_dir)
	gcc -c ./render/src/scene.c $(INCLUDES) -o $@

$(render_dir)/fog.o: ./render/src/fog.c ./render/include/render.h $(render_dir)
	gcc -c ./render/src/fog.c $(INCLUDES) -o $@

$(render_dir)/render.o: ./render/src/render.c ./render/include/render.h ./render/include/color.h $(render_dir)
	gcc -c ./render/src/render.c $(INCLUDES) -o $@

$(render_dir)/triangle.o: ./render/src/triangle.c ./render/include/render.h ./render/include/color.h $(render_dir)
	gcc -c ./render/src/triangle.c $(INCLUDES) -o $@

$(render_dir)/sphere.o: ./render/src/sphere.c ./render/include/render.h ./render/include/color.h $(render_dir)
	gcc -c ./render/src/sphere.c $(INCLUDES) -o $@

$(render_dir)/kdtree.o: ./render/src/kdtree.c ./render/include/kdtree.h ./render/include/render.h $(render_dir)
	gcc -c ./render/src/kdtree.c $(INCLUDES) -o $@

$(render_lib): $(render_dir)/render.o $(render_dir)/triangle.o $(render_dir)/sphere.o $(render_dir)/kdtree.o $(render_dir)/scene.o $(render_dir)/fog.o $(render_dir)/canvas.o
	ar -rcs $@ $^

#
# Routines
#
.PHONY: clean
clean:
	rm -f *.o;                            \
	rm -f ./test ./test_gl ./test_kd;     \
	rm -f *.mp4;                          \
	rm -rf *.dSYM                         \
	rm -rf $(render_dir) $(frame_dir)
