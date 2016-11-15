// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "view.hpp"


/******************* CAMERA STRUCT MEMBER FUNCTION DEFINITIONS ****************/

void Camera::setup() {
    DEBUG_assert(this->near > 0);
    DEBUG_assert(this->left < this->right);
    DEBUG_assert(this->bottom < this->top);
    DEBUG_assert(this->near < this->far);

    glViewport(0, 0, this->x_res, this->y_res);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (this->type) {
        case VP_FOV:
            gluPerspective(
                this->fov,
                (float) this->x_res / this->y_res,
                this->near,
                this->far);
            break;
        case VP_FRUSTUM:
            glFrustum(
                this->left,
                this->right,
                this->bottom,
                this->top,
                this->near,
                this->far);
            break;
        case VP_DEFAULT:
        default:
            gluOrtho2D(this->left, this->right, this->bottom, this->top);
            break;
    }

    DEBUG_printf("Applying Cam...\n");
    DEBUG_printf("\tposition: (%f, %f, %f)\n", position[0], position[1], position[2]);
    DEBUG_printf("\tlookat: (%f, %f, %f)\n", lookat[0], lookat[1], lookat[2]);
    DEBUG_printf("\taxis: (%f, %f, %f)\n", axis[0], axis[1], axis[2]);
    DEBUG_printf("\tup: (%f, %f, %f)\n", up[0], up[1], up[2]);
    DEBUG_printf("\tfov: %f\n", fov);
    DEBUG_printf("\tleft: %f\n", left);
    DEBUG_printf("\tright: %f\n", right);
    DEBUG_printf("\tbottom: %f\n", bottom);
    DEBUG_printf("\ttop: %f\n", top);
    DEBUG_printf("\tnear: %f\n", near);
    DEBUG_printf("\tfar: %f\n", far);
    DEBUG_printf("\ttype: %d\n", (int) type);
    DEBUG_printf("\tresolution: %d x %d\n", x_res, y_res);

    // switch back to model view matrix mode to be safe
    glMatrixMode(GL_MODELVIEW);
}

const float Camera::screenWidth() const {
    switch (this->type) {
        case VP_FOV:
            return screenHeight() * (float) this->x_res / (float) this->y_res;
        case VP_FRUSTUM:
        case VP_DEFAULT:
        default:
            return this->right - this->left;
    }
}

const float Camera::screenHeight() const {
    switch (this->type) {
        case VP_FOV:
            return 2 * this->near * tan(this->fov * M_PI / 360.0);
        case VP_FRUSTUM:
        case VP_DEFAULT:
        default:
            return this->top - this->bottom;
    }
}

// function for syncing mouse's pixel coordinate and screen coordinate
void syncMouseCoords(Mouse &mouse, const Camera &cam) {
    mouse.screen_x =
        ((float) mouse.pixel_x / (float) cam.x_res - 0.5) * cam.screenWidth();
    mouse.screen_y =
        ((float) mouse.pixel_y / (float) cam.y_res - 0.5) * cam.screenHeight();
}

// default camera
const Camera default_cam = {
    default_cam_position,
    default_cam_lookat,
    default_cam_axis,
    default_cam_up,
    default_cam_fov,
    default_cam_left,
    default_cam_right,
    default_cam_bottom,
    default_cam_top,
    default_cam_near,
    default_cam_far,
    VP_DEFAULT,
    default_cam_x_res,
    default_cam_y_res,
};

/********************************** View Class ********************************/

View::View(const char *display_title) :
    cam(default_cam),
    mouse(),
    display_title(display_title)
{

}

View::~View() {
    // TODO: is this needed?
    // delete this->display_title;
}

// opengl functions (VIRTUAL DEFAULT CALLBACK FUNC)
void View::setupFunc() {
    // TODO: this breaks the display i think
    if (glutGetWindow() != 0) {
        glutDestroyWindow(glutGetWindow());
    }

    glutInitWindowSize(this->cam.x_res, this->cam.y_res);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(this->display_title);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("NOT GLEW_OK: %d\n", err);
        exit(1); // or handle the error in a nicer way
    }
    if (!GLEW_VERSION_2_1) { // check that the machine supports the 2.1 API.
        printf("NOT GLEW_VERSION_2_1: %d\n", err);
        exit(1); // or handle the error in a nicer way
    }

    DEBUG_printf("SETUP! Window set to %d\n", glutGetWindow());
}

void View::reshapeFunc(int width, int height) {
    int old_x_res = this->cam.x_res;
    int old_y_res = this->cam.y_res;

    this->cam.x_res = (width == 0) ? 1 : width;
    this->cam.y_res = (height == 0) ? 1 : height;

    this->cam.left *= (float) this->cam.x_res / old_x_res;
    this->cam.right *= (float) this->cam.x_res / old_x_res;
    this->cam.bottom *= (float) this->cam.y_res / old_y_res;
    this->cam.top *= (float) this->cam.y_res / old_y_res;

    this->cam.setup();
}

void View::displayFunc() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

void View::mousePressedFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        this->mouse.pixel_x = x;
        this->mouse.pixel_y = y;
        syncMouseCoords(this->mouse, this->cam);
        this->mouse.is_pressed = true;
    } else if (state == GLUT_UP) {
        if (button == GLUT_LEFT_BUTTON) {
            DEBUG_printf("LEFT CLICK AT: p(%d, %d)\t|\ts(%f, %f)\n",
                this->mouse.pixel_x, this->mouse.pixel_y,
                this->mouse.screen_x, this->mouse.screen_y);
        } else if (button == GLUT_RIGHT_BUTTON) {
            DEBUG_printf("RIGHT CLICK AT: p(%d, %d)\t|\ts(%f, %f)\n",
                this->mouse.pixel_x, this->mouse.pixel_y,
                this->mouse.screen_x, this->mouse.screen_y);
        }
        this->mouse.is_pressed = false;
    }
}

void View::mouseMovedFunc(int x, int y) {
    if (this->mouse.is_pressed) {
        this->mouse.pixel_x = x;
        this->mouse.pixel_y = y;
        syncMouseCoords(this->mouse, this->cam);
    }
}

void View::keyPressedFunc(unsigned char key, int x, int y) {
    DEBUG_printf("KEY PRESS: mod = %d, key = %c, x = %d, y = %d\n",
        glutGetModifiers(), key, x, y);
}

void View::keyReleasedFunc(unsigned char key, int x, int y) {
    DEBUG_printf("KEY RELEASE: mod = %d, key = %c, x = %d, y = %d\n",
        glutGetModifiers(), key, x, y);
}

void View::specKeyPressedFunc(int key, int x, int y) {
    DEBUG_printf("SPEC KEY PRESS: mod = %d, key = %d, x = %d, y = %d\n",
        glutGetModifiers(), key, x, y);
}

void View::specKeyReleasedFunc(int key, int x, int y) {
    DEBUG_printf("SPEC KEY RELEASE: mod = %d, key = %d, x = %d, y = %d\n",
        glutGetModifiers(), key, x, y);
}

