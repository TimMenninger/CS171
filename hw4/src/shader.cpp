// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "shader.hpp"

#include <iostream>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include "file_util.hpp"

using namespace glm;

std::vector<Shader> Shader::shaders;

Shader::Shader() : shader_obj() {}
Shader::~Shader() {}

int Shader::load(
    const char *vertex_shader_filename,
    const char *fragment_shader_filename)
{
    int new_shader_id = Shader::shaders.size();

    Shader::shaders.push_back(Shader());
    if (!Shader::shaders.rbegin()->generateShaderObj(vertex_shader_filename,
                                                    fragment_shader_filename))
    {
        fprintf(stderr, "ERROR generating shader object [%s, %s]\n",
            vertex_shader_filename,
            fragment_shader_filename);
        return k_invalid_index;
    }

    return new_shader_id;
}

bool Shader::apply(int index) {
    if (index > k_invalid_index && index < (int) Shader::shaders.size()) {
        glUseProgram(Shader::shaders[index].shader_obj);
        return true;
    }
    return false;
}

bool Shader::setUniformVariable(
    int index,
    const char *u_var_name,
    GLSLUniformVariableType type,
    void *val)
{
    if (index > k_invalid_index && index < (int) Shader::shaders.size()) {
        GLint u_var_loc =
            glGetUniformLocation(
                Shader::shaders[index].shader_obj,
                u_var_name);
        if (u_var_loc != -1) {
            switch (type) {
                case UV_int:
                    glProgramUniform1i(
                        index,
                        u_var_loc,
                        *((const GLint*) val));
                    break;
                case UV_float:
                    glProgramUniform1f(
                        index,
                        u_var_loc,
                        *((const GLfloat*) val));
                    break;
                case UV_mat_4f:
                    glProgramUniformMatrix4fv(
                        index,
                        u_var_loc,
                        1,
                        GL_FALSE,
                        (const GLfloat*) val);
                    break;
                default:
                    return false;
            }
        } else {
            fprintf(stderr, "ERROR: COULD NOT FIND UNIFORM VARIABLE: %s\n",
                u_var_name);
            return false;
        }
    }
    return false;
}

/******************************************************************************/

bool Shader::generateShaderObj(
    const char *vertex_shader_filename,
    const char *fragment_shader_filename)
{
    GLchar *shader_src_v, *shader_src_f;
    int len_v, len_f;
    int load_err;

    load_err = loadFile(vertex_shader_filename, &shader_src_v, &len_v);
    switch (load_err) {
        case 0:
            DEBUG_printf("loaded file %s\n", vertex_shader_filename);
            break;
        case -1:
            fprintf(stderr, "couldn't open file %s\n", vertex_shader_filename);
            return false;
        case -2:
            fprintf(stderr, "empty file %s\n", vertex_shader_filename);
            return false;
        case -3:
            fprintf(stderr, "not enough memory. len = %d\n", len_v);
            return false;
    }

    load_err = loadFile(fragment_shader_filename, &shader_src_f, &len_f);
    switch (load_err) {
        case 0:
            DEBUG_printf("loaded file %s\n", fragment_shader_filename);
            break;
        case -1:
            fprintf(stderr, "couldn't open file %s\n", fragment_shader_filename);
            return false;
        case -2:
            fprintf(stderr, "empty file %s\n", fragment_shader_filename);
            return false;
        case -3:
            fprintf(stderr, "not enough memory. len = %d\n", len_f);
            return false;
    }

    GLuint shader_obj_v, shader_obj_f;

    shader_obj_v = glCreateShader(GL_VERTEX_SHADER);
    shader_obj_f = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shader_obj_v, 1, (const GLchar**) &shader_src_v, &len_v);
    glShaderSource(shader_obj_f, 1, (const GLchar**) &shader_src_f, &len_f);

    glCompileShader(shader_obj_v);
    glCompileShader(shader_obj_f);

    GLint compiled;
    glGetShaderiv(shader_obj_v, GL_COMPILE_STATUS, &compiled);
    if (compiled) {
        DEBUG_printf("vertex shader compiled\n");
    } else {
        GLint blen = 0; 
        GLsizei slen = 0;

        glGetShaderiv(shader_obj_v, GL_INFO_LOG_LENGTH , &blen);       
        if (blen > 1) {
            GLchar* compiler_log = (GLchar*) malloc(blen);
            glGetInfoLogARB(shader_obj_v, blen, &slen, compiler_log);
            DEBUG_printf("compiler_log: %s\n", compiler_log);
            free(compiler_log);
        }
        return false;
    }
    glGetShaderiv(shader_obj_f, GL_COMPILE_STATUS, &compiled);
    if (compiled) {
        DEBUG_printf("fragment shader compiled\n");
    } else {
        GLint blen = 0; 
        GLsizei slen = 0;

        glGetShaderiv(shader_obj_f, GL_INFO_LOG_LENGTH , &blen);       
        if (blen > 1) {
            GLchar* compiler_log = (GLchar*) malloc(blen);
            glGetInfoLogARB(shader_obj_f, blen, &slen, compiler_log);
            DEBUG_printf("compiler_log: %s\n", compiler_log);
            free(compiler_log);
        }
        return false;
    }

    unloadFile((GLubyte**) &shader_src_v);
    unloadFile((GLubyte**) &shader_src_f);

    this->shader_obj = glCreateProgram();

    glAttachShader(this->shader_obj, shader_obj_v);
    glAttachShader(this->shader_obj, shader_obj_f);

    glLinkProgram(this->shader_obj);

    GLint linked;
    glGetProgramiv(this->shader_obj, GL_LINK_STATUS, &linked);
    if (linked) {
        DEBUG_printf("successfully linked shader obj\n");
    } else {
        fprintf(stderr, "could not link shader obj\n");
        glDeleteProgram(this->shader_obj);
        return false;
    }

    glDetachShader(this->shader_obj, shader_obj_v);
    glDetachShader(this->shader_obj, shader_obj_f);

    glDeleteShader(shader_obj_v);
    glDeleteShader(shader_obj_f);

    return true;
}
