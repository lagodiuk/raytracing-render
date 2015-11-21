CC = gcc

INCLUDES = $(addprefix -I, ./include)

ifeq ($(shell uname),Darwin)
  INCLUDES += -I/usr/X11/include
  INCLUDES += -I/usr/local/include
endif

CFLAGS = $(DEF) -Wall
CFLAGS += -std=gnu89 -O2
#
# http://www.insidepro.com/kk/231/231r.shtml
# Boosting render performance a bit :)
#CFLAGS += -O3 -ffast-math

lib_dir = ./lib

render_lib = $(lib_dir)/librender.a

$(lib_dir):
	mkdir -p $@

$(lib_dir)/obj_loader.o: ./src/obj_loader.c ./include/obj_loader.h ./include/queue.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/obj_loader.c -o $@

$(lib_dir)/canvas.o: ./src/canvas.c ./include/canvas.h ./include/color.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -fopenmp -c ./src/canvas.c -o $@

$(lib_dir)/scene.o: ./src/scene.c ./include/render.h ./include/color.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/scene.c -o $@

$(lib_dir)/fog.o: ./src/fog.c ./include/render.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/fog.c -o $@

$(lib_dir)/tracer.o: ./src/tracer.c ./include/render.h ./include/color.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/tracer.c -o $@

$(lib_dir)/render.o: ./src/render.c ./include/render.h ./include/color.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -fopenmp -c ./src/render.c -o $@

$(lib_dir)/triangle.o: ./src/triangle.c ./include/render.h ./include/color.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/triangle.c -o $@

$(lib_dir)/sphere.o: ./src/sphere.c ./include/render.h ./include/color.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/sphere.c -o $@

$(lib_dir)/kdtree.o: ./src/kdtree.c ./include/kdtree.h ./include/render.h $(lib_dir)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/kdtree.c -o $@

render: $(lib_dir)/tracer.o $(lib_dir)/render.o $(lib_dir)/triangle.o $(lib_dir)/sphere.o $(lib_dir)/kdtree.o $(lib_dir)/scene.o $(lib_dir)/fog.o $(lib_dir)/canvas.o $(lib_dir)/obj_loader.o
	ar -rcs $(render_lib) $^

.PHONY: clean
clean:
	rm -rf $(lib_dir)
