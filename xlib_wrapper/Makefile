
CFLAGS = -shared -undefined dynamic_lookup
LIBS = -lX11 -lm

clean:
	rm -f libxlib_wrapper.so

build:
	cp xlib_wrapper.h ../testing/
	clang xlib_wrapper.c -o libxlib_wrapper.so $(CFLAGS) $(LIBS)
