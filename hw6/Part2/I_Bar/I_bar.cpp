/******************************************************************************

 I_bar.cpp

 Contains code for drawing I-bar animation

 Author: Tim Menninger

******************************************************************************/

#include "I_bar.h"

using namespace std;
using namespace Eigen;

// Initialization functions for OpenGL display
void init(void);

// Relevant for displaying objects properly
void drawIBar();

// Callback functions we supply to OpenGL
void reshape(int width, int height);
void display(void);
void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

///////////////////////////////////////////////////////////////////////////////

/* Self-explanatory lists of lights and objects.
 */

// The camera view information
Camera cam;
// X and Y resolutions
int xres, yres;
// Time step
double h;

// Rotation matrices used for the ability to rotate the view
MatrixXd currentRotation(4, 4);
MatrixXd lastRotation(4, 4);

int mouse_x, mouse_y;
int mouse_x0, mouse_y0;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

/* Needed to draw the cylinders using glu */
GLUquadricObj *quadratic;


///////////////////////////////////////////////////////////////////////////////////////////////////

/* From here on are all the function implementations.
 */


/* Initializes OpenGL variables
 */
void init(void)
{
    quadratic = gluNewQuadric();

    /* The following line of code tells OpenGL to use "smooth shading" (aka
     * Gouraud shading) when rendering.
     */
    glShadeModel(GL_SMOOTH);

    /* The next line of code tells OpenGL to use "culling" when rendering.
     */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /* The following line tells OpenGL to use depth buffering when rendering.
     */
    glEnable(GL_DEPTH_TEST);

     /* The following line tells OpenGL to automatically normalize our normal
     * vectors before it passes them into the normal arrays.
     */
    glEnable(GL_NORMALIZE);

    /* The following two lines tell OpenGL to enable its "vertex array" and
     * "normal array" functionality.
     */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Initialize camera values
    initCamera(&cam);

    /* The next 4 lines work with OpenGL's two main matrices: the "Projection
     * Matrix" and the "Modelview Matrix".
     */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(cam.left_param, cam.right_param,
              cam.bottom_param, cam.top_param,
              cam.near_param, cam.far_param);

    /* The Modelview Matrix is the matrix that OpenGL applies to untransformed
     * points in world space.
     */
    glMatrixMode(GL_MODELVIEW);

    /* The next line calls our function that tells OpenGL to initialize some
     * lights to represent our Point Light structs.
     */
    init_lights();

    // Initialize our rotation matrices to the identity, which essentially
    // corresponds to our rotation being none yet
    currentRotation = MatrixXd::Identity(4, 4);
    lastRotation = MatrixXd::Identity(4, 4);

    drawIBar();
}

/* 'reshape' function
 */
void reshape(int width, int height)
{
    /* The following two lines of code prevent the width and height of the
     * window from ever becoming 0 to prevent divide by 0 errors later.
     * Typically, we let 1x1 square pixel be the smallest size for the window.
     */
    height = (height == 0) ? 1 : height;
    width = (width == 0) ? 1 : width;

    /* The 'glViewport' function tells OpenGL to determine how to convert from
     * NDC to screen coordinates given the dimensions of the window. The
     * parameters for 'glViewport' are (in the following order):
     *
     * - int x: x-coordinate of the lower-left corner of the window in pixels
     * - int y: y-coordinate of the lower-left corner of the window in pixels
     * - int width: width of the window
     * - int height: height of the window
     */
    glViewport(0, 0, width, height);

    /* The following two lines are specific to updating our mouse interface
     * parameters.
     */
    mouse_scale_x = (float) (cam.right_param - cam.left_param) / (float) width;
    mouse_scale_y = (float) (cam.top_param - cam.bottom_param) / (float) height;

    /* The following line tells OpenGL that our program window needs to
     * be re-displayed, meaning everything that was being displayed on
     * the window before it got resized needs to be re-rendered.
     */
    glutPostRedisplay();
}

/* 'display' function:
 *
 * The 'display' function is supposed to handle all the processing of points
 * in world and camera space.
 */
void display(void)
{
    /* The following line of code is typically the first line of code in any
     * 'display' function.
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* With the program window cleared, OpenGL is ready to render a new scene.
     */
    glLoadIdentity();

    /* Our next step is to specify the inverse rotation of the camera by its
     * orientation angle about its orientation axis:
     */
    glRotatef(-cam.cam_orientation_angle,
              cam.cam_orientation_axis[0],
              cam.cam_orientation_axis[1],
              cam.cam_orientation_axis[2]);
    /* We then specify the inverse translation of the camera by its position using
     * the 'glTranslatef' function, which takes the following parameters in the
     * following order:
     *
     * - float x: x-component of translation vector
     * - float y: x-component of translation vector
     * - float z: x-component of translation vector
     */
    glTranslatef(-cam.cam_position[0], -cam.cam_position[1], -cam.cam_position[2]);
    glRotatef(y_view_angle, 1, 0, 0);
    glRotatef(x_view_angle, 0, 1, 0);

    // Multiply the matrix by the rotation matrix that is a result
    // of clicking and dragging on the screen
    MatrixXd q = currentRotation * lastRotation;
    double qDbl[16] = { q(0, 0), q(0, 1), q(0, 2), q(0, 3),
                        q(1, 0), q(1, 1), q(1, 2), q(1, 3),
                        q(2, 0), q(2, 1), q(2, 2), q(2, 3),
                        q(3, 0), q(3, 1), q(3, 2), q(3, 3) };
    glMultMatrixd(qDbl);

    /* Our next step is to set up all the lights in their specified positions.
     */
    set_lights();
    /* Once the lights are set, we can specify the points and faces that we
     * want drawn.
     */
    drawIBar();

    glutSwapBuffers();
}

/* 'init_lights' function
 */
void init_lights()
{
    glEnable(GL_LIGHTING);

    int num_lights = lights.size();

    for(int i = 0; i < num_lights; ++i)
    {
        int light_id = GL_LIGHT0 + i;

        glEnable(light_id);

        /* The following lines of code use 'glLightfv' to set the color of
         * the light. The parameters for 'glLightfv' are:
         *
         * - enum light_ID: an integer between 'GL_LIGHT0' and 'GL_LIGHT7'
         * - enum property: this varies depending on what you are setting
         *                  e.g. 'GL_AMBIENT' for the light's ambient component
         * - float* values: a set of values to set for the specified property
         *                  e.g. an array of RGB values for the light's color
         */
        glLightfv(light_id, GL_AMBIENT, lights[i].color);
        glLightfv(light_id, GL_DIFFUSE, lights[i].color);
        glLightfv(light_id, GL_SPECULAR, lights[i].color);

        /* The following line of code sets the attenuation k constant of the
         * light.
         */
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, lights[i].attenuation_k);
    }
}

/* 'set_lights' function
 */
void set_lights()
{
    int num_lights = lights.size();

    for(int i = 0; i < num_lights; ++i)
    {
        int light_id = GL_LIGHT0 + i;

        glLightfv(light_id, GL_POSITION, lights[i].position);
    }
}

/* 'drawIBar' function:
 *
 * This function has OpenGL render our objects to the display screen. It
 */
 void drawIBar()
 {
     /* Parameters for drawing the cylinders */
     float cyRad = 0.2, cyHeight = 1.0;
     int quadStacks = 4, quadSlices = 4;

     glPushMatrix();
     glColor3f(0, 0, 1);
     glTranslatef(0, cyHeight, 0);
     glRotatef(90, 1, 0, 0);
     gluCylinder(quadratic, cyRad, cyRad, 2.0 * cyHeight, quadSlices, quadStacks);
     glPopMatrix();

     glPushMatrix();
     glColor3f(0, 1, 1);
     glTranslatef(0, cyHeight, 0);
     glRotatef(90, 0, 1, 0);
     gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
     glPopMatrix();

     glPushMatrix();
     glColor3f(1, 0, 1);
     glTranslatef(0, cyHeight, 0);
     glRotatef(-90, 0, 1, 0);
     gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
     glPopMatrix();

     glPushMatrix();
     glColor3f(1, 1, 0);
     glTranslatef(0, -cyHeight, 0);
     glRotatef(-90, 0, 1, 0);
     gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
     glPopMatrix();

     glPushMatrix();
     glColor3f(0, 1, 0);
     glTranslatef(0, -cyHeight, 0);
     glRotatef(90, 0, 1, 0);
     gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
     glPopMatrix();
 }

/* 'mouse_pressed' function
 */
void mouse_pressed(int button, int state, int x, int y)
{
    /* If the left-mouse button was clicked down, then...
     */
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        /* Store the mouse position in our global variables.
         */
        mouse_x = x;
        mouse_y = y;

        /* Store the mouse position as the initial position, so we know how
         * to rotate the scene if dragged.
         */
        mouse_x0 = x;
        mouse_y0 = y;

        /* Since the mouse is being pressed down, we set our 'is_pressed"
         * boolean indicator to true.
         */
        is_pressed = true;
    }
    /* If the left-mouse button was released up, then...
     */
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // Record our rotation as the last rotation, and reset our current
        // rotation for the next time we rotate the screen
        lastRotation = currentRotation * lastRotation;
        currentRotation = MatrixXd::Identity(4, 4);

        /* Mouse is no longer being pressed, so set our indicator to false.
         */
        is_pressed = false;
    }
}

/* 'computeRotationQuaternion' function:
 *
 * This function computes the rotation matrix given the starting x and y points
 * and the current x and y points of the mouse as it drags.
 */
MatrixXd computeRotationQuaternion ()
{
    // We need to first convert our points to NDC which is just scaling them
    // from (0, res) to (-1, 1).
    double xNDC  = ((float) mouse_x / xres) - 1;
    double yNDC  = ((float) mouse_y / yres) - 1;
    double x0NDC = ((float) mouse_x0 / xres) - 1;
    double y0NDC = ((float) mouse_y0 / yres) - 1;

    // We now want to estimate what the z coordinates are for each NDC point
    double zNDC = 0;
    if (xNDC*xNDC + yNDC*yNDC <= 1)
        zNDC = sqrt(1 - xNDC*xNDC - yNDC*yNDC);
    double z0NDC = 0;
    if (x0NDC*x0NDC + y0NDC*y0NDC <= 1)
        z0NDC = sqrt(1 - x0NDC*x0NDC - y0NDC*y0NDC);

    // To compute theta, we must dot the two points in NDC and divide by the
    // product of their magnitudes.
    double pDotP0 = xNDC*x0NDC + yNDC*y0NDC + zNDC*z0NDC;
    double magP  = sqrt(xNDC*xNDC + yNDC*yNDC + zNDC*zNDC);
    double magP0 = sqrt(x0NDC*x0NDC + y0NDC*y0NDC + z0NDC*z0NDC);
    double theta = acos(min(1.0, pDotP0 / (magP * magP0)));

    // To compute our rotation matrix, we need a normalized vector u, which is
    // the cross product of our two points.  We will compute u component-wise.
    double uX = z0NDC * yNDC - y0NDC * zNDC;
    double uY = x0NDC * zNDC - z0NDC * xNDC;
    double uZ = y0NDC * xNDC - x0NDC * yNDC;
    double magU = sqrt(uX*uX + uY*uY + uZ*uZ);
    uX /= magU;
    uY /= magU;
    uZ /= magU;

    // Compute the unit quaternion
    double qS = cos(theta / 2);
    double qX = uX * sin(theta / 2);
    double qY = uY * sin(theta / 2);
    double qZ = uZ * sin(theta / 2);

    // Using these, we can now generate our quaternion
    MatrixXd quat(4, 4);
    quat << 1-2*(qY*qY+qZ*qZ),   2*(qX*qY-qZ*qS),   2*(qX*qZ+qY*qS), 0,
              2*(qX*qY+qZ*qS), 1-2*(qX*qX+qZ*qZ),   2*(qY*qZ-qX*qS), 0,
              2*(qX*qZ-qY*qS),   2*(qY*qZ+qX*qS), 1-2*(qX*qX+qY*qY), 0,
                            0,                 0,                 0, 1;

    return quat;
}

/* 'mouse_moved' function:
 *
 * This function is meant to respond to when the mouse is being moved. There
 * are just two parameters to this function:
 *
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * We compute our camera rotation angles based on the mouse movement in this
 * function.
 */
void mouse_moved(int x, int y)
{
    /* If the left-mouse button is being clicked down...
     */
    if(is_pressed)
    {
        x_view_angle += ((float) x - (float) mouse_x) * mouse_scale_x * x_view_step;
        float temp_y_view_angle = y_view_angle +
                                  ((float) y - (float) mouse_y) * mouse_scale_y * y_view_step;
        y_view_angle = (temp_y_view_angle > 90 || temp_y_view_angle < -90) ?
                       y_view_angle : temp_y_view_angle;

        /* We update our 'mouse_x' and 'mouse_y' variables so that if the user moves
         * the mouse again without releasing it, then the distance we compute on the
         * next call to the 'mouse_moved' function will be from this current mouse
         * position.
         */
        mouse_x = x;
        mouse_y = y;

        /* Compute how to change the image to reflect the changing of view
         */
        currentRotation = computeRotationQuaternion();

        /* Tell OpenGL that it needs to re-render our scene with the new camera
         * angles.
         */
        glutPostRedisplay();
    }
}

/* 'deg2rad' function:
 *
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

/* 'key_pressed' function
 */
void key_pressed(unsigned char key, int x, int y)
{
    /* If 'q' is pressed, quit the program.
     */
    if(key == 'q')
    {
        exit(0);
    }
    /* If 't' is pressed, toggle our 'wireframe_mode' boolean to make OpenGL
     * render our cubes as surfaces of wireframes.
     */
    else if(key == 't')
    {
        wireframe_mode = !wireframe_mode;
        /* Tell OpenGL that it needs to re-render our scene with the cubes
         * now as wireframes (or surfaces if they were wireframes before).
         */
        glutPostRedisplay();
    }
    else if(key == 'n')
    {
        // TODO: Next frame

        glutPostRedisplay();
    }
    else
    {
        float x_view_rad = deg2rad(x_view_angle);

        /* 'w' for step forward
         */
        if(key == 'w')
        {
            cam.cam_position[0] += step_size * sin(x_view_rad);
            cam.cam_position[2] -= step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        /* 'a' for step left
         */
        else if(key == 'a')
        {
            cam.cam_position[0] -= step_size * cos(x_view_rad);
            cam.cam_position[2] -= step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
        /* 's' for step backward
         */
        else if(key == 's')
        {
            cam.cam_position[0] -= step_size * sin(x_view_rad);
            cam.cam_position[2] += step_size * cos(x_view_rad);
            glutPostRedisplay();
        }
        /* 'd' for step right
         */
        else if(key == 'd')
        {
            cam.cam_position[0] += step_size * cos(x_view_rad);
            cam.cam_position[2] += step_size * sin(x_view_rad);
            glutPostRedisplay();
        }
    }
}

/* The 'main' function:
 *
 * This function is short, but is basically where everything comes together.
 */
int main(int argc, char* argv[])
{
    if (argc != 4) {
        cout << "usage: ./ibar [frames.script] [xres] [yres]"
             << endl << "    frames.script: the frames in the movie"
             << endl << "    xres: x resolution"
             << endl << "    yres: y resolution"
             << endl;
        return 1;
    }
    xres = atoi(argv[2]);
    yres = atoi(argv[3]);

    Movie movie;
    if (importMovie(argv[1], &movie))
        // If returns nonzero, there was an error
        return 1;

    /* 'glutInit' intializes the GLUT (Graphics Library Utility Toolkit) library.
     * This is necessary, since a lot of the functions we used above and below
     * are from the GLUT library.
     *
     * 'glutInit' takes the 'main' function arguments as parameters. This is not
     * too important for us, but it is possible to give command line specifications
     * to 'glutInit' by putting them with the 'main' function arguments.
     */
    glutInit(&argc, argv);
    /* The following line of code tells OpenGL that we need a double buffer,
     * a RGB pixel buffer, and a depth buffer.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /* The following line tells OpenGL to create a program window of size
     * 'xres' by 'yres'.
     */
    glutInitWindowSize(xres, yres);
    /* The following line tells OpenGL to set the program window in the top-left
     * corner of the computer screen (0, 0).
     */
    glutInitWindowPosition(0, 0);
    /* The following line tells OpenGL to name the program window "Test".
     */
    glutCreateWindow("I-Bar");

    /* Call our 'init' function...
     */
    init();
    /* Specify to OpenGL our display function.
     */
    glutDisplayFunc(display);
    /* Specify to OpenGL our reshape function.
     */
    glutReshapeFunc(reshape);
    /* Specify to OpenGL our function for handling mouse presses.
     */
    glutMouseFunc(mouse_pressed);
    /* Specify to OpenGL our function for handling mouse movement.
     */
    glutMotionFunc(mouse_moved);
    /* Specify to OpenGL our function for handling key presses.
     */
    glutKeyboardFunc(key_pressed);
    /* The following line tells OpenGL to start the "event processing loop". This
     * is an infinite loop where OpenGL will continuously use our display, reshape,
     * mouse, and keyboard functions to essentially run our program.
     */
    glutMainLoop();

    return 0;
}
