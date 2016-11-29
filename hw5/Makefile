###############################################################################
# CS/CNS 171 Fall 2015
#
# Makefile for smooth
#
# Tim Menninger
###############################################################################
CC = g++
FLAGS = -g -o

INCLUDE = -I/usr/X11R6/include -I/usr/include/GL -I/usr/include -Ilib/
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
SOURCES = src/*.cpp
LIBS = -lGLEW -lGL -lGLU -lglut -lm
OPTS = -Wno-deprecated

BIN = bin/
EXENAME = $(BIN)smooth

all: $(SOURCES)
	$(CC) $(FLAGS) $(EXENAME) $(OPTS) $(INCLUDE) $(LIBDIR) $(SOURCES) $(LIBS)

again: clean all

clean:
	rm -f *.o $(EXENAME)
	rm -rf $(BIN)*.dSYM
	rm -f *.DS_Store

.PHONY: all clean
