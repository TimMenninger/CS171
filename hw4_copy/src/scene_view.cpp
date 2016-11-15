// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "scene_view.hpp"

#include "scene.hpp"
#include "shader.hpp"
#include "light.hpp"
#include "renderer.hpp"
#include "model.hpp"

/******************************* SceneView Class ******************************/

SceneView::SceneView(const char *display_title, Scene *scene) :
    View(display_title),
    scene(scene),
    shader_id(k_invalid_index),
    cam_angle(),
    cam_angle_velocity()
{
    //
}

SceneView::~SceneView() {
    //
}

void SceneView::updateCamPosition() {
    this->cam_angle += this->cam_angle_velocity;
    while (this->cam_angle > 3.14159) {
        this->cam_angle -= 2 * 3.14159;
    }
    while (this->cam_angle < -3.14159) {
        this->cam_angle += 2 * 3.14159;
    }
    this->cam_angle_velocity /= 1.02;
    this->cam.position[0] = 10.0 * cos(cam_angle);
    this->cam.position[1] = 0.0;
    this->cam.position[2] = 10.0 * sin(cam_angle);
}

void SceneView::updateCamLookat() {
    this->cam.lookat[0] = 0.0;
    this->cam.lookat[1] = 0.0;
    this->cam.lookat[2] = 0.0;
}

// opengl functions (VIRTUAL DEFAULT CALLBACK FUNC)
void SceneView::setupFunc() {
    // set fullscreen and setup camera
    View::setupFunc();

    glutFullScreen();
    int screen_xres = glutGet(GLUT_WINDOW_WIDTH);
    int screen_yres = glutGet(GLUT_WINDOW_HEIGHT);
    this->cam.x_res = screen_xres;
    this->cam.y_res = screen_yres;
    this->cam.near = 1.0;
    this->cam.far = 1000.0;
    this->cam.type = VP_FOV;
    this->cam.fov = 30.0;
    this->cam.setup();

    // setup space
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // setup shader
    this->shader_id = Shader::load(
        "shaders/basic_phong_v.glsl",
        "shaders/basic_phong_f.glsl");
    if (this->shader_id != k_invalid_index) {
        Shader::apply(this->shader_id);
    }
}

void SceneView::displayFunc() {
    DEBUG_assert(this->scene != NULL);

    glPushMatrix();

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // point camera
    glLoadIdentity();
    gluLookAt(this->cam.position[0],
            this->cam.position[1],
            this->cam.position[2],
            this->cam.lookat[0],
            this->cam.lookat[1],
            this->cam.lookat[2],
            this->cam.up[0],
            this->cam.up[1],
            this->cam.up[2]);

    // render and swap buffers
    this->render();
    glutSwapBuffers();

    glPopMatrix();
}

void SceneView::keyPressedFunc(unsigned char key, int x, int y) {
    View::keyPressedFunc(key, x, y);

    switch (key) {
        // system commands
        case 27:    // ESC
            exit(0);
        case 'd':
            this->cam_angle_velocity += 0.05;
            if (this->cam_angle_velocity > 0.2) {
                this->cam_angle_velocity = 0.2;
            }
            break;
        case 'a':
            this->cam_angle_velocity -= 0.05;
            if (this->cam_angle_velocity < -0.2) {
                this->cam_angle_velocity = -0.2;
            }
            break;
    }
}

void SceneView::render() {
    glPushMatrix();

    for (const SceneObject &object : scene->getSceneObjects()) {
        glTranslatef(
            object.position[0],
            object.position[1],
            object.position[2]);
        glRotatef(
            object.rotation[0],
            object.rotation[1],
            object.rotation[2],
            object.rotation[3]);
        glScalef(object.scale[0], object.scale[1], object.scale[2]);

        renderModel(Model::getModel(object.model_id));
    }

    glPopMatrix();
}

void SceneView::update(int usecs) {
    // update camera position and axis
    this->updateCamPosition();
    this->updateCamLookat();
}
