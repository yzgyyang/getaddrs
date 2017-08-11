CFLAGS?=-Wall -O2 -g

getaddrs: main.c lib1.so lib2.so
	cc ${CFLAGS} -o -std=c99 ${.TARGET} -Wl,--rpath=. -L. -l 1 -l 2 main.c

.SUFFIXES: .so
.c.so:
	cc ${CFLAGS} -fPIC -shared -o ${.TARGET} ${.IMPSRC}

.PHONY:	clean
clean:
	rm getaddrs lib1.so lib2.so
