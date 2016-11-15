#ifndef SCENE_HPP
#define SCENE_HPP

#include "common.hpp"

struct SceneObject {
    int model_id;
    Vector3f position;
    Vector4f rotation;
    Vector3f scale;
};

class Scene {
private:
    std::vector<int> light_ids;
    std::vector<SceneObject> objects;

public:
    Scene();
    ~Scene();

    const std::vector<SceneObject> &getSceneObjects();

    void addLight(
        GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat pos_w,
        GLfloat amb_r, GLfloat amb_g, GLfloat amb_b, GLfloat amb_a,
        GLfloat diff_r, GLfloat diff_g, GLfloat diff_b, GLfloat diff_a,
        GLfloat spec_r, GLfloat spec_g, GLfloat spec_b, GLfloat spec_a,
        GLfloat att_constant, GLfloat att_linear, GLfloat att_quadratic);

    void addSceneObject(
        int model_id,
        const Vector3f &position,
        const Vector4f &rotation,
        const Vector3f &scale);
};

#endif
