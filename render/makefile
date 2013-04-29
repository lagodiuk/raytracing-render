lib_dir 	=	./lib
render_lib	=	$(lib_dir)/librender.a
INCLUDES	=	-O2 $(addprefix -I, ./include)

$(lib_dir):
	mkdir -p $@

$(lib_dir)/thread_pool.o: ./src/thread_pool.c ./include/queue.h ./include/thread_pool.h $(lib_dir)
	gcc -c ./src/thread_pool.c $(INCLUDES) -lpthread -o $@

$(lib_dir)/canvas.o: ./src/canvas.c ./include/canvas.h ./include/color.h $(lib_dir)
	gcc -c ./src/canvas.c $(INCLUDES) -o $@

$(lib_dir)/scene.o: ./src/scene.c ./include/render.h ./include/color.h $(lib_dir)
	gcc -c ./src/scene.c $(INCLUDES) -o $@

$(lib_dir)/fog.o: ./src/fog.c ./include/render.h $(lib_dir)
	gcc -c ./src/fog.c $(INCLUDES) -o $@

$(lib_dir)/render.o: ./src/render.c ./include/render.h ./include/color.h $(lib_dir)
	gcc -c ./src/render.c $(INCLUDES) -o $@

$(lib_dir)/triangle.o: ./src/triangle.c ./include/render.h ./include/color.h $(lib_dir)
	gcc -c ./src/triangle.c $(INCLUDES) -o $@

$(lib_dir)/sphere.o: ./src/sphere.c ./include/render.h ./include/color.h $(lib_dir)
	gcc -c ./src/sphere.c $(INCLUDES) -o $@

$(lib_dir)/kdtree.o: ./src/kdtree.c ./include/kdtree.h ./include/render.h $(lib_dir)
	gcc -c ./src/kdtree.c $(INCLUDES) -o $@

$(render_lib): $(lib_dir)/render.o $(lib_dir)/triangle.o $(lib_dir)/sphere.o $(lib_dir)/kdtree.o $(lib_dir)/scene.o $(lib_dir)/fog.o $(lib_dir)/canvas.o $(lib_dir)/thread_pool.o
	ar -rcs $@ $^

.PHONY: clean
clean:
	rm -rf $(lib_dir)