#ifndef MODEL_HPP
#define MODEL_HPP

#include "common.hpp"

struct Vertex {
    GLfloat coord[3];
    GLfloat normal[3];
    GLfloat uv[2];
};

struct Material {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat emission[4];
    GLfloat shininess;
};

class Model {
private:
    static std::vector<Model> models;

    GLuint vbo_vertex_id;
    GLuint vbo_index_id;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Material material;

    explicit Model();

public:
    ~Model();

    static int createModel();
    static Model &getModel(int model_id);

    const GLuint getVBOVertexID();
    const GLuint getVBOIndexID();
    std::vector<Vertex> &getVertices();
    std::vector<GLuint> &getIndices();
    const Material &getMaterial();

    void loadObjFile(const char *filename);
    void bind();

    void setAmbient(
        const GLfloat r,
        const GLfloat g,
        const GLfloat b,
        const GLfloat a);
    void setDiffuse(
        const GLfloat r,
        const GLfloat g,
        const GLfloat b,
        const GLfloat a);
    void setSpecular(
        const GLfloat r,
        const GLfloat g,
        const GLfloat b,
        const GLfloat a);
    void setEmission(
        const GLfloat r,
        const GLfloat g,
        const GLfloat b,
        const GLfloat a);
    void setShininess(const GLfloat shininess);
};


#endif
