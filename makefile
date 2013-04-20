UNAME := $(shell uname)

render_dir 	=	./render/lib
render_lib	=	$(render_dir)/librender.a

LIBPATH		=	$(addprefix -L, $(render_dir))
INCLUDES	=	-O2 $(addprefix -I, ./render/headers)
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

$(render_dir)/canvas.o: ./render/source/canvas.c ./render/headers/canvas.h ./render/headers/color.h $(render_dir)
	gcc -c ./render/source/canvas.c $(INCLUDES) -o $@

$(render_dir)/scene.o: ./render/source/scene.c ./render/headers/render.h ./render/headers/color.h $(render_dir)
	gcc -c ./render/source/scene.c $(INCLUDES) -o $@

$(render_dir)/fog.o: ./render/source/fog.c ./render/headers/render.h $(render_dir)
	gcc -c ./render/source/fog.c $(INCLUDES) -o $@

$(render_dir)/render.o: ./render/source/render.c ./render/headers/render.h ./render/headers/color.h $(render_dir)
	gcc -c ./render/source/render.c $(INCLUDES) -o $@

$(render_dir)/triangle.o: ./render/source/triangle.c ./render/headers/render.h ./render/headers/color.h $(render_dir)
	gcc -c ./render/source/triangle.c $(INCLUDES) -o $@

$(render_dir)/sphere.o: ./render/source/sphere.c ./render/headers/render.h ./render/headers/color.h $(render_dir)
	gcc -c ./render/source/sphere.c $(INCLUDES) -o $@

$(render_dir)/kdtree.o: ./render/source/kdtree.c ./render/headers/kdtree.h ./render/headers/render.h $(render_dir)
	gcc -c ./render/source/kdtree.c $(INCLUDES) -o $@

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
