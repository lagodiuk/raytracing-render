LIBPATH	 = -Lrender/lib
INCLUDES = -Irender/include
LIBS = -lrender -lm -lpng -fopenmp

CC_OPTS	 = -Wall -O2

render = render/lib/librender.a

benchmark: $(render) benchmark.c
	gcc $(CC_OPTS) -fopenmp benchmark.c $(LIBPATH) $(INCLUDES) $(LIBS) -o $@	

example: $(render) example.c
	gcc $(CC_OPTS) -fopenmp example.c $(LIBPATH) $(INCLUDES) $(LIBS) -o $@

run_demo_gl: $(render)
	(cd demo && make DEF="$(DEF)" run_demo_gl)

$(render):
	(cd render && make DEF="$(DEF)" render)

.PHONY: clean
clean:
	(cd render && make clean) && \
	(cd demo && make clean)   && \
	rm -f ./example ./benchmark;		\
	rm -f *.png		
