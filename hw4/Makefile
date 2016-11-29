CXX = g++
CXXFLAGS = -std=c++11 -g -Wall -pedantic -O3

INCLUDE = -I/usr/include/GL -I/usr/include -Ilib
LIBDIR = -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu
LIBS = -lGLEW -lGL -lGLU -lglut -lm -lboost_system -lboost_chrono

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
ETC_DIR = etc

MAIN_SRC =	$(OBJ_DIR)/main.o
MAIN_SRC +=	$(OBJ_DIR)/scene.o
MAIN_SRC +=	$(OBJ_DIR)/display.o
MAIN_SRC +=	$(OBJ_DIR)/shader.o
MAIN_SRC +=	$(OBJ_DIR)/light.o
MAIN_SRC +=	$(OBJ_DIR)/view.o
MAIN_SRC +=	$(OBJ_DIR)/scene_view.o
MAIN_SRC +=	$(OBJ_DIR)/renderer.o
MAIN_SRC +=	$(OBJ_DIR)/model.o
MAIN_SRC +=	$(OBJ_DIR)/timer.o

MAIN_EXE = $(BIN_DIR)/shader

TARGETS = $(MAIN_EXE)

all: $(TARGETS)

$(MAIN_EXE): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBDIR) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGETS)

again: clean all

.PHONY: all clean


