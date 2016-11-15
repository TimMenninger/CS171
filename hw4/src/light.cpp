// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "light.hpp"

const int Light::k_max_light_count = 4;
Light *Light::g_light = new Light[k_max_light_count];

const GLfloat Light::g_global_ambient[4] = {0.0, 0.0, 0.0, 1.0};

static const GLfloat k_default_position[4] = {0.0, 0.0, 0.0, 1.0};
static const GLfloat k_default_ambient[4] = {0.2, 0.2, 0.2, 1.0};
static const GLfloat k_default_diffuse[4] = {0.5, 0.5, 0.5, 1.0};
static const GLfloat k_default_specular[4] = {0.7, 0.7, 0.7, 1.0};
static const GLfloat k_default_attenuation_constant = 1.0;
static const GLfloat k_default_attenuation_linear = 0.0;
static const GLfloat k_default_attenuation_quadratic = 0.0;

Light::Light() :
    active(false),
    type(LS_default),
    position(),
    ambient(),
    diffuse(),
    specular(),
    attenuation_constant(k_default_attenuation_constant),
    attenuation_linear(k_default_attenuation_linear),
    attenuation_quadratic(k_default_attenuation_quadratic),
    shadow_shader_obj(NULL),
    shadow_depth_fbo(NULL),
    shadow_depth_texture(NULL),
    shadow_buffer_cam()
{
    position[0] = k_default_position[0];
    position[1] = k_default_position[1];
    position[2] = k_default_position[2];
    position[3] = k_default_position[3];

    ambient[0] = k_default_ambient[0];
    ambient[1] = k_default_ambient[1];
    ambient[2] = k_default_ambient[2];
    ambient[3] = k_default_ambient[3];

    diffuse[0] = k_default_diffuse[0];
    diffuse[1] = k_default_diffuse[1];
    diffuse[2] = k_default_diffuse[2];
    diffuse[3] = k_default_diffuse[3];

    specular[0] = k_default_specular[0];
    specular[1] = k_default_specular[1];
    specular[2] = k_default_specular[2];
    specular[3] = k_default_specular[3];
}

Light::~Light() {
    this->clear();
}

void Light::setup(LightType type) {
    glEnable(GL_LIGHT0 + this->gl_light_index);

    this->active = true;
    this->type = type;

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light::g_global_ambient);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

    glLightf(GL_LIGHT0 + this->gl_light_index, GL_CONSTANT_ATTENUATION,
        this->attenuation_constant);
    glLightf(GL_LIGHT0 + this->gl_light_index, GL_LINEAR_ATTENUATION,
        this->attenuation_linear);
    glLightf(GL_LIGHT0 + this->gl_light_index, GL_QUADRATIC_ATTENUATION,
        this->attenuation_quadratic);

    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_POSITION, this->position);
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_AMBIENT, this->ambient);
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_DIFFUSE, this->diffuse);
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_SPECULAR, this->specular);
}

void Light::clear() {
    this->active = false;
    // TODO
}

void Light::setPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    DEBUG_assert(this->active);
    this->position[0] = x;
    this->position[1] = y;
    this->position[2] = z;
    this->position[3] = w;
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_POSITION, this->position);
}

void Light::setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    DEBUG_assert(this->active);
    this->ambient[0] = r;
    this->ambient[1] = g;
    this->ambient[2] = b;
    this->ambient[3] = a;
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_AMBIENT, this->ambient);
}

void Light::setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    DEBUG_assert(this->active);
    this->diffuse[0] = r;
    this->diffuse[1] = g;
    this->diffuse[2] = b;
    this->diffuse[3] = a;
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_DIFFUSE, this->diffuse);
}

void Light::setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    DEBUG_assert(this->active);
    this->specular[0] = r;
    this->specular[1] = g;
    this->specular[2] = b;
    this->specular[3] = a;
    glLightfv(GL_LIGHT0 + this->gl_light_index, GL_SPECULAR, this->specular);
}

void Light::setAttenuation(GLfloat constant, GLfloat linear, GLfloat quadratic) {
    DEBUG_assert(this->active);
    this->attenuation_constant = constant;
    this->attenuation_linear = linear;
    this->attenuation_quadratic = quadratic;

    glLightf(GL_LIGHT0 + this->gl_light_index, GL_CONSTANT_ATTENUATION,
        this->attenuation_constant);
    glLightf(GL_LIGHT0 + this->gl_light_index, GL_LINEAR_ATTENUATION,
        this->attenuation_linear);
    glLightf(GL_LIGHT0 + this->gl_light_index, GL_QUADRATIC_ATTENUATION,
        this->attenuation_quadratic);
}

void Light::init() {
    glEnable(GL_LIGHTING);
    for (int i = 0; i < k_max_light_count; i++) {
        g_light[i].gl_light_index = i;
        g_light[i].clear();
    }
}

int Light::create(LightType type) {
    for (int i = 0; i < k_max_light_count; i++) {
        if (!g_light[i].active) {
            g_light[i].setup(type);
            return i;
        }
    }
    return k_invalid_index;
}

Light *Light::get(int index) {
    if (index > k_invalid_index && index < k_max_light_count
        && g_light[index].active) {
        return g_light + index;
    }
    return NULL;
}

void Light::deactivate(int index) {
    if (g_light[index].active) {
        g_light[index].clear();
    } else {
        printf("WARNING: request to deactivate inactive light %d\n", index);
    }
}
