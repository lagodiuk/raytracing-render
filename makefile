UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CC_OPTS_TEST_GL = -framework GLUT -framework OpenGL -DDARWIN
endif

ifeq ($(UNAME), Linux)
CC_OPTS_TEST_GL = -lglut -DPOSIX
endif

# Generating by gcc in Mac OS
d_sym	= *.dSYM

render_dir 	=	./render/lib
render_lib	=	$(render_dir)/librender.a

canvas_dir	=	./canvas/lib
canvas_lib 	=	$(canvas_dir)/libcanvas.a

LIBPATH		=	$(addprefix -L, $(render_dir) $(canvas_dir))
INCLUDES	=	$(addprefix -I, ./render/headers ./canvas/headers) 
LINKLIBS	= 	-lcanvas -lrender -lm
CC_OPTS		=	-Wall -O2 $(CC_OPTS_TEST_GL) -DN_WORKERS=$(NCPU)

frame_dir	=	./frames

#
# Test applications
#

test_video: test $(frame_dir)
	cd $(frame_dir) && ../test
	ffmpeg -qscale 2 -r 10 -b 10M  -i '$(frame_dir)/out_%03d.bmp'  movie.mp4

test: test.c scene1.h scene1.o $(canvas_lib) $(render_lib)
	gcc -O test.c scene1.o $(CC_OPTS) $(INCLUDES) $(LIBPATH) $(LINKLIBS) -o $@

rungl: test_gl
	./$<

test_gl: $(canvas_lib) $(render_lib) scene1.o mt_render.o mt_render.h scene1.h test_gl.c makefile
	gcc -pthread test_gl.c scene1.o mt_render.o $(CC_OPTS) $(INCLUDES) $(LIBPATH) $(LINKLIBS) -o $@

$(frame_dir):
	mkdir -p $@

scene1.o: scene1.c
	gcc -c $< $(INCLUDES) $(CC_OPTS) -o $@

mt_render.o: mt_render.c
	gcc -c $< $(INCLUDES) $(CC_OPTS) -o $@

#
# Canvas
#

$(canvas_dir):
	mkdir -p $@

$(canvas_dir)/canvas.o: ./canvas/source/canvas.c ./canvas/headers/canvas.h ./canvas/headers/color.h $(canvas_dir)
	gcc -c ./canvas/source/canvas.c -I./canvas/headers/ -o $@

$(canvas_lib): $(canvas_dir)/canvas.o
	ar -rcs $@ $^

#
# Render
#

$(render_dir):
	mkdir -p $@

$(render_dir)/render.o: ./render/source/render.c ./render/headers/render.h ./canvas/headers/color.h $(render_dir)
	gcc -c ./render/source/render.c $(INCLUDES) -o $@

$(render_dir)/triangle.o: ./render/source/triangle.c ./render/headers/render.h ./canvas/headers/color.h $(render_dir)
	gcc -c ./render/source/triangle.c $(INCLUDES) -o $@

$(render_dir)/kdtree.o: ./render/source/kdtree.c ./render/headers/kdtree.h ./render/headers/render.h $(render_dir)
	gcc -c ./render/source/kdtree.c $(INCLUDES) -o $@

$(render_lib): $(render_dir)/render.o $(render_dir)/triangle.o $(render_dir)/kdtree.o
	ar -rcs $@ $^

#
# Routines
#
.PHONY: clean
clean:
	rm -f *.o; \
	rm -f ./test ./test_gl ./test_kd;     \
	rm -f *.mp4;                \
	rm -rf $(canvas_dir) $(render_dir) $(frame_dir) $(d_sym)
