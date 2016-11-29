// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "scene.hpp"

#include "light.hpp"
#include "model.hpp"

Scene::Scene() : light_ids(), objects() {
    this->begin = clock();
}

Scene::~Scene() {
    //
}

const std::vector<SceneObject> &Scene::getSceneObjects() {
    return this->objects;
}

void Scene::addLight(
    GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat pos_w,
    GLfloat amb_r, GLfloat amb_g, GLfloat amb_b, GLfloat amb_a,
    GLfloat diff_r, GLfloat diff_g, GLfloat diff_b, GLfloat diff_a,
    GLfloat spec_r, GLfloat spec_g, GLfloat spec_b, GLfloat spec_a,
    GLfloat att_constant, GLfloat att_linear, GLfloat att_quadratic)
{
    int new_id = Light::create(LS_default);
    Light::get(new_id)->setPosition(pos_x, pos_y, pos_z, pos_w);
    Light::get(new_id)->setAmbient(amb_r, amb_g, amb_b, amb_a);
    Light::get(new_id)->setDiffuse(diff_r, diff_g, diff_b, diff_a);
    Light::get(new_id)->setSpecular(spec_r, spec_g, spec_b, spec_a);
    Light::get(new_id)->setAttenuation(att_constant, att_linear, att_quadratic);

    this->light_ids.push_back(new_id);
}

void Scene::addSceneObject(
    int model_id,
    const Vector3f &position,
    const Vector4f &rotation,
    const Vector3f &scale)
{
    DEBUG_assert(model_id != k_invalid_index);
    this->objects.push_back({model_id, position, rotation, scale});
}

//
// Scene::elapsedUsecs
//
// Returns the amount of time since the start of this program.
//
// Arguments: None.
// Returns:   (int) - Number of microseconds since program start
//
// Revisions: 11/15/16 - Tim Menninger: Created
//
int Scene::elapsedUsecs() {
    // Return usecs since last function call
    return double(clock() - this->begin) * 1000000 / CLOCKS_PER_SEC;
}
