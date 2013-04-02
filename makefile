clean:
	rm -f ./test
	rm -f *.bmp
	rm -f *.mp4
	rm -rf ./canvas/lib
	rm -rf ./render/lib

#
# Canvas
#

color.o: ./canvas/source/color.c ./canvas/headers/color.h
	mkdir -p ./canvas/lib
	gcc -c ./canvas/source/color.c -I./canvas/headers/ -o ./canvas/lib/color.o

canvas.o: ./canvas/source/canvas.c ./canvas/headers/canvas.h ./canvas/headers/color.h
	mkdir -p ./canvas/lib
	gcc -c ./canvas/source/canvas.c -I./canvas/headers/ -o ./canvas/lib/canvas.o

canvas_lib: color.o canvas.o
	ar -rcs ./canvas/lib/libcanvas.a ./canvas/lib/color.o ./canvas/lib/canvas.o
	rm ./canvas/lib/color.o
	rm ./canvas/lib/canvas.o

#
# Render
#

render.o: ./render/source/render.c ./render/headers/render.h ./canvas/headers/color.h
	mkdir -p ./render/lib
	gcc -c ./render/source/render.c -I./render/headers/ -I./canvas/headers/ -o ./render/lib/render.o

utils.o: ./render/source/utils.c ./render/headers/utils.h ./render/headers/render.h
	mkdir -p ./render/lib
	gcc -c ./render/source/utils.c -I./render/headers/ -I./canvas/headers/ -o ./render/lib/utils.o

triangle.o: ./render/source/triangle.c ./render/headers/render.h ./canvas/headers/color.h
	mkdir -p ./render/lib
	gcc -c ./render/source/triangle.c -I./render/headers/ -I./canvas/headers/ -o ./render/lib/triangle.o

render_lib: render.o utils.o triangle.o
	ar -rcs ./render/lib/librender.a ./render/lib/render.o ./render/lib/utils.o ./render/lib/triangle.o
	rm ./render/lib/render.o
	rm ./render/lib/utils.o
	rm ./render/lib/triangle.o

#
# Test application
#

test: test.c canvas_lib render_lib
	gcc -O test.c -I./render/headers/ -I./canvas/headers/ -L./render/lib/ -L./canvas/lib/ -lcanvas -lrender -o test

test_video: test
	./test
	ffmpeg -qscale 2 -r 10 -b 10M  -i 'out_%03d.bmp'  movie.mp4
