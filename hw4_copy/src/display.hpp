#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "common.hpp"

class View;
class Timer;

/******************************** Display Class *******************************/

// class for Display (OpenGL based displays) objects
class Display {
private:
    // is extra default implementations worth it?
    static Display *display;
    static bool opengl_is_setup;

    // access to world information and view through which to display it
    View *view;

    // enable glutTimerFunc?
    Timer *timer;

    explicit Display();
    ~Display();

    // static callback functions passed to OpenGL
    static void reshapeCallback(int width, int height);
    static void displayCallback();
    static void mousePressedCallback(int button, int state, int x, int y);
    static void mouseMovedCallback(int x, int y);
    static void keyPressedCallback(unsigned char key, int x, int y);
    static void keyReleasedCallback(unsigned char key, int x, int y);
    static void specKeyPressedCallback(int key, int x, int y);
    static void specKeyReleasedCallback(int key, int x, int y);
    static void timerCallback(int usecs);

public:
    // static instance controller functions
    static Display *instance();
    static void setup(int argc, char **argv, Timer *timer);
    static void reset();
    static void destroy();

    void setView(View *view);
    void clearView();
    void start();
    void update(int usecs);
};

#endif