###############################################################################
# CS/CNS 171 Fall 2015
#
# This is a template Makefile for the assignment. Edit it however you find
# convenient.
###############################################################################
CC = g++ -std=c++11
FLAGS = -g
SOURCES = *.cpp

EXENAME = objParser

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(SOURCES)

clean:
	rm -f *.o $(EXENAME)
	rm -rf *.dSYM

.PHONY: all clean
