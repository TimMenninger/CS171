###############################################################################
# CS/CNS 171 Fall 2015
#
# This is a template Makefile for OpenGL programs. Edit it however you find
# convenient.
#
# The current version of this file should compile OpenGL programs just fine on
# Debian-based Linux operating systems.
#
# If you run Mac OS or other distributions of Linux, then you may have to
# fool around with the 'INCLUDE' and 'LIBDIR' lines below before the Makefile
# can compile the OpenGL parts successfully.
###############################################################################
CC = g++ -std=c++11
FLAGS = -g -o

INCLUDE = -I/usr/X11R6/include -I/usr/include/GL -I/usr/include -Ilib/
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
SOURCES = src/*.cpp
LIBS = -lGLEW -lGL -lGLU -lglut -lm
OPTS = -Wno-deprecated

BIN = bin/
EXENAME = $(BIN)opengl_shader

all: $(SOURCES)
	$(CC) $(FLAGS) $(EXENAME) $(OPTS) $(INCLUDE) $(LIBDIR) $(SOURCES) $(LIBS)

clean:
	rm -f *.o $(EXENAME)
	rm -fr $(BIN)*.dSYM

.PHONY: all clean
