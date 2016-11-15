#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.hpp"

enum GLSLUniformVariableType {
    UV_int,
    UV_float,
    UV_mat_4f,
    UV_type_count
};

class Shader {
private:
    static std::vector<Shader> shaders;

    GLuint shader_obj;

    explicit Shader();
    explicit Shader(
        const char *vertex_shader_filename,
        const char *fragment_shader_filename);

    bool generateShaderObj(
        const char *vertex_shader_filename,
        const char *fragment_shader_filename);

public:
    ~Shader();

    static int load(
        const char *vertex_shader_filename,
        const char *fragment_shader_filename);
    static bool apply(int index);
    static bool setUniformVariable(
        int index,
        const char *u_var_name,
        GLSLUniformVariableType type,
        void *val);
};

#endif