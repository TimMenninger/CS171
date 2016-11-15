#ifndef SCENE_VIEW_HPP
#define SCENE_VIEW_HPP

#include "common.hpp"

#include "view.hpp"

class Scene;

class SceneView : public View {
private:
    Scene *scene;
    int shader_id;

    float cam_angle;
    float cam_angle_velocity;

    explicit SceneView();
    // ^ disabled

    // update camera position
    void updateCamPosition();
    void updateCamLookat();

public:
    explicit SceneView(const char *display_title, Scene *scene);
    ~SceneView();

    // derived functionality
    void render();
    void update(int usecs);

    // virtual function for setting up opengl display
    void setupFunc();
    // derived (Display) virtual functions called in the callback functions
    void displayFunc();
    void keyPressedFunc(unsigned char key, int x, int y);
};

#endif
