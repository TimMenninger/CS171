#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "common.hpp"
#include "view.hpp"

// GLfloat spot_direction[] = {1.0, -1.0, -1.0};
// GLint spot_exponent = 64;
// GLint spot_cutoff = 10;

// glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
// glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
// glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
// ^^?????

enum LightType {
    LS_unidirectional,
    LS_spherical,
    LS_default = LS_unidirectional,
    LS_type_count
};

class Light {
private:
    static const int k_max_light_count;
    static Light *g_light;

    static const GLfloat g_global_ambient[4];

    bool active;

    GLuint gl_light_index;
    LightType type;

    GLfloat position[4];
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat attenuation_constant;
    GLfloat attenuation_linear;
    GLfloat attenuation_quadratic;

    GLuint *shadow_shader_obj;
    GLuint *shadow_depth_fbo;
    GLuint *shadow_depth_texture;

    Camera shadow_buffer_cam;

    explicit Light();
    ~Light();

    void setup(LightType type);
    void clear();

public:
    static void init();
    static int create(LightType type);
    static Light *get(int index);
    static void deactivate(int index);

    void setPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void setAttenuation(GLfloat constant, GLfloat linear, GLfloat quadratic);
};

#endif
