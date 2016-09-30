#include <iostream>
#include <assert.h>

#define CIRCLE_COLOR    { 255,   0,   0 }
#define BG_COLOR        { 255, 255, 255 }
#define MAX_INTENSITY   255

void newImage (int, int, int*, int***);
void drawCircle (int, int, int, int, int, int*, int***);
