#
# Trivial Makefile to build the "Geanyuniq" Geany plugin.
#

BASENAME=geanyuniq

CFLAGS=`pkg-config --cflags geany` -fPIC -Wall -pedantic -O3
LDLIBS=`pkg-config --libs geany`

OBJS=bloomfilter.o murmurhash.o

PLUGINDIR=`pkg-config --variable=libdir geany`/geany

.PHONY:		clean install install-dev

# --------------------------------------------------------------

$(BASENAME).so:	$(BASENAME).o $(OBJS)
		gcc -shared -o $@ $(LDLIBS) $^

$(BASENAME).o:	$(BASENAME).c

bloomfilter.o:	bloomfilter.c bloomfilter.h

murmurhash.o:	murmurhash.c murmurhash.h

# --------------------------------------------------------------

clean:
	rm -f *.o *.so


# Installation for "end users", who don't want to keep the source around.
install:
	cp $(BASENAME).so $(PLUGINDIR)


# Installation for developers, who want to be able to iterate quickly.
install-dev:
	ln -s `pwd`/$(BASENAME).so $(PLUGINDIR)
