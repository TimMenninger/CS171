#ifndef VIEW_HPP
#define VIEW_HPP

#include "common.hpp"

enum view_port_t {VP_DEFAULT, VP_FRUSTUM, VP_FOV};

// struct for camera object
// holds camera location and shape information
struct Camera {
    // camera location and orientation
    Vector3f position;
    Vector3f lookat;
    Vector3f axis;
    Vector3f up;

    // view port dimensions (in screen space (float))
    float fov;
    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;

    view_port_t type;

    // resolution (in pixels)
    int x_res;
    int y_res;

    // apply camera info to OpenGL
    void setup();

    // returns screen Width and Height
    // i.e. right_param - left_param and top_param - bottom_param
    // NOTE THIS IS WORLD SPACE
    const float screenWidth() const;
    const float screenHeight() const;
};

// struct for mouse object
// holds mouse location and response behavior
struct Mouse {
    int pixel_x;
    int pixel_y;
    float screen_x;
    float screen_y;
    bool is_pressed = false;
};

// function for syncing mouse's pixel coordinate and screen coordinate
void syncMouseCoords(Mouse &mouse, const Camera &cam);

/********************************** View Class ********************************/

class View {
protected:
    Camera cam;
    Mouse mouse;

    // disable default constructor
    explicit View();

public:
    const char *display_title;
    
    explicit View(const char *display_title);
    virtual ~View();

    virtual void render() = 0;
    virtual void update(int usecs) = 0;

    // virtual function for setting up opengl display
    virtual void setupFunc();
    // virtual functions called in the callback functions
    virtual void reshapeFunc(int width, int height);
    virtual void displayFunc();
    virtual void mousePressedFunc(int button, int state, int x, int y);
    virtual void mouseMovedFunc(int x, int y);
    virtual void keyPressedFunc(unsigned char key, int x, int y);
    virtual void keyReleasedFunc(unsigned char key, int x, int y);
    virtual void specKeyPressedFunc(int key, int x, int y);
    virtual void specKeyReleasedFunc(int key, int x, int y);
};


#endif
