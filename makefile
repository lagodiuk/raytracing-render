clean:
	rm -f *.o
	rm -f ./test
	rm -f *.bmp
	rm -f *.mp4

color.o: color.c color.h
	gcc -c color.c -I. -o color.o

canvas.o: canvas.c canvas.h color.h
	gcc -c canvas.c -I. -o canvas.o

geometry3d.o: geometry3d.c geometry3d.h color.h
	gcc -c geometry3d.c -I. -o geometry3d.o

test: test.c color.o geometry3d.o canvas.o
	gcc -O test.c color.o geometry3d.o canvas.o -I. -o test
	./test

test_video: test.c color.o geometry3d.o canvas.o
	gcc -O test.c color.o geometry3d.o canvas.o -I. -o test
	./test
	ffmpeg -qscale 2 -r 10 -b 10M  -i 'out_%03d.bmp'  movie.mp4
