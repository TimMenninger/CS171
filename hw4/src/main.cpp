#define DEBUG_PRINT
#define DEBUG_ASSERT

#include "main.hpp"

#include "scene.hpp"
#include "display.hpp"
#include "view.hpp"
#include "scene_view.hpp"
#include "model.hpp"
#include "timer.hpp"

void registerUpdateTasks(Timer *timer);

int main(int argc, char **argv) {
    // Setup Scene
    Scene *scene = new Scene();

    int model_id = Model::createModel();
    Model::getModel(model_id).loadObjFile("data/bunny.obj");
    Model::getModel(model_id).setAmbient(0.1, 0.1, 0.1, 1.0);
    Model::getModel(model_id).setDiffuse(0.4, 0.4, 0.4, 1.0);
    Model::getModel(model_id).setSpecular(0.8, 0.8, 0.8, 1.0);
    Model::getModel(model_id).setEmission(0.0, 0.0, 0.0, 1.0);
    Model::getModel(model_id).setShininess(10);

    scene->addSceneObject(
        model_id,
        {0.0, -1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0},
        {3.0, 3.0, 3.0});

    // Setup timer
    Timer *timer = new Timer();

    DEBUG_printf("Display init...\t\t\t");
    // Setup display
    Display::setup(argc, argv, timer);
    DEBUG_printf("...DONE.\n");

    DEBUG_printf("View setup...\t\t\t");
    View *scene_view = new SceneView("CS171 HW4: Shaders", scene);
    Display::instance()->setView(scene_view);
    DEBUG_printf("...DONE.\n");

    DEBUG_printf("task register...\t\t");
    registerUpdateTasks(timer);
    DEBUG_printf("...DONE.\n");

    Model::getModel(model_id).bind();

    // start display
    Display::instance()->start();

    return 0;
}

void displayUpdate(int usec);
void gameUpdate(int usec);

void registerUpdateTasks(Timer *timer) {
    timer->registerTask(0, displayUpdate);
}

void displayUpdate(int usec) {
    Display::instance()->update(usec);
}
