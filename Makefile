CFLAGS?=-Wall -O2 -g

getaddrs: main.c lib1.so lib2.so
	cc ${CFLAGS} -o ${.TARGET} -std=c99 -Wl,--rpath=. -L. -l 1 -l 2 main.c

.SUFFIXES: .so
.c.so:
	cc ${CFLAGS} -fPIC -shared -o ${.TARGET} ${.IMPSRC}

.PHONY:	clean
clean:
	rm getaddrs lib1.so lib2.so
