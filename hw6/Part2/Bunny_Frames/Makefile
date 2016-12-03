###############################################################################
# CS/CNS 171 Fall 2016
#
# Makefile for interpolate.
###############################################################################
CC = g++
FLAGS = -std=c++11 -g -Wno-deprecated -pthread

INCLUDE =-I/usr/include
LIBDIR = -L/usr/local/lib
SOURCES = *.cpp
LIBS =

EXENAME = interpolate

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(LIBDIR) $(SOURCES) $(LIBS)

clean:
	rm -f *.o $(EXENAME)
	rm -rf *.dSYM

.PHONY: all clean
