// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "display.hpp"

#include "view.hpp"

/******************* DISPLAY CLASS MEMBER FUNCTION DEFINITIONS ****************/

Display *Display::display = NULL;
bool Display::opengl_is_setup = false;

Display::Display() : view(NULL), timer(NULL) {

}

Display::~Display() {
    if (this->timer) {
        delete this->timer;
    }
    this->clearView();
}

// opengl functions (STATIC CALLBACK)
void Display::reshapeCallback(int width, int height) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->reshapeFunc(width, height);
}
void Display::displayCallback() {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->displayFunc();
}
void Display::mousePressedCallback(int button, int state, int x, int y) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->mousePressedFunc(button, state, x, y);
}
void Display::mouseMovedCallback(int x, int y) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->mouseMovedFunc(x, y);
}
void Display::keyPressedCallback(unsigned char key, int x, int y) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->keyPressedFunc(key, x, y);
}
void Display::keyReleasedCallback(unsigned char key, int x, int y) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->keyReleasedFunc(key, x, y);
}
void Display::specKeyPressedCallback(int key, int x, int y) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->specKeyPressedFunc(key, x, y);
}
void Display::specKeyReleasedCallback(int key, int x, int y) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    display->view->specKeyReleasedFunc(key, x, y);
}
void Display::timerCallback(int usecs) {
    DEBUG_assert(Display::display);
    DEBUG_assert(Display::display->view);
    DEBUG_assert(Display::display->timer);

    // trigger timer
    Display::display->timer->trigger();
    
    glutTimerFunc(desired_interval, Display::timerCallback,
        Display::display->timer->checkClock(TI_MICRO));
}

Display *Display::instance() {
    return Display::display;
}

void Display::setup(int argc, char **argv, Timer *timer) {
    DEBUG_assert(timer != NULL);

    if (Display::display == NULL) {
        Display::display = new Display();
    }

    if (!opengl_is_setup) {
        DEBUG_printf("Setting up Display (OpenGL)\n");
        // setup opengl
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

        DEBUG_printf("\tBase setup...\t\t\tDONE.\n");

        // setup Timer
        Display::display->timer = timer;

        DEBUG_printf("\tTimer setup...\t\t\tDONE.\n");

        // toggle opengl_is_setup
        opengl_is_setup = true;
    } else {
        printf("WARNING: OpenGL setup called twice!\n");
    }
}

void Display::reset() {
    if (Display::display != NULL) {
        delete Display::display;
        Display::display = NULL;
    }
}

void Display::destroy() {
    if (Display::display != NULL) {
        delete Display::display;
    }
}

void Display::setView(View *view) {
    DEBUG_assert(view != NULL);
    DEBUG_assert(this->timer != NULL);
    DEBUG_printf("Setting View: %s\n", view->display_title)

    // TODO: probably should delete old view? unless i come up with a better
    // control flow for this process
    DEBUG_assert(Display::display != NULL);
    this->view = view;
    this->view->setupFunc();
}

void Display::clearView() {
    if (this->view != NULL) {
        delete this->view;
        this->view = NULL;
    }
}

void Display::start() {
    DEBUG_assert(this->view);
    DEBUG_assert(this->timer);

    // setup gl functions
    glutDisplayFunc(Display::displayCallback);
    glutReshapeFunc(Display::reshapeCallback);
    glutMouseFunc(Display::mousePressedCallback);
    glutMotionFunc(Display::mouseMovedCallback);
    glutKeyboardFunc(Display::keyPressedCallback);
    glutKeyboardUpFunc(Display::keyReleasedCallback);
    glutSpecialFunc(Display::specKeyPressedCallback);
    glutSpecialUpFunc(Display::specKeyReleasedCallback);

    glutTimerFunc(desired_interval, Display::timerCallback,
        this->timer->checkClock(TI_MICRO));

    glutMainLoop();
}

void Display::update(int usecs) {
    DEBUG_assert(this->view);
    DEBUG_assert(this->timer);

    Display::display->view->update(usecs);

    // display
    glutPostRedisplay();
}
