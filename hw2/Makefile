###############################################################################
# CS 171
# Tim Menninger
# 10/21/16
#
# Makefile for HW2, which contains shading algorithms
###############################################################################
CC = g++
FLAGS = -g -std=c++11 -Wno-deprecated

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
INCLUDE = -I../ -Ilib/
SRCDIR = src
SOURCES = $(SRCDIR)/*.cpp

EXENAME = bin/shaded

all: $(EXENAME)

$(EXENAME): $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(SOURCES)

clean:
	rm -f *.o $(EXENAME)
	rm -rf bin/*.dSYM

.PHONY: all clean
