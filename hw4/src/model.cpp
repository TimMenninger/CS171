// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "model.hpp"

/********************************* Model Class ********************************/

std::vector<Model> Model::models;

int Model::createModel() {
    int new_model_id = Model::models.size();
    Model::models.push_back(Model());
    return new_model_id;
}

Model &Model::getModel(int model_id) {
    if (model_id < (int) Model::models.size()) {
        return models[model_id];
    } else {
        fprintf(stderr,
            "ERROR Model::get model for Model ID %d does not exist\n",
            model_id);
        exit(1);
    }
}

Model::Model() :
    vbo_vertex_id(),
    vbo_index_id(),
    vertices(),
    indices(),
    material()
{
    //
}

Model::~Model() {
    this->vertices.clear();
    this->indices.clear();
}

const GLuint Model::getVBOVertexID() {
    return this->vbo_vertex_id;
}

const GLuint Model::getVBOIndexID() {
    return this->vbo_index_id;
}

std::vector<Vertex> &Model::getVertices() {
    return this->vertices;
}

std::vector<GLuint> &Model::getIndices() {
    return this->indices;
}

const Material &Model::getMaterial() {
    return this->material;
}

// To keep things simple, this function assumes that there is one normal per
// vertex. This is usually the case for simplistic surfaces with Euler
// Characteristic of 2. Thankfully most meshes can be represented as polyhedrons
// with no holes.
// There are other assumptions made so make sure the input OBJ files follow the
// strict requirements of this function.
void Model::loadObjFile(const char *filename) {
    DEBUG_assert(this->vertices.empty());
    DEBUG_assert(this->indices.empty());

    const int buffer_size = 256;
    char buffer[buffer_size];
    std::ifstream obj_file(filename);
    if (obj_file.is_open()) {
        int cursor = obj_file.tellg();

        this->vertices.push_back(Vertex());

        // parse vertices
        while (obj_file.getline(buffer, buffer_size)) {
            char *data_entry_type = strtok(buffer, " ");
            if (strcmp(data_entry_type, "v") == 0) {
                char *val0 = strtok(NULL, " ");
                char *val1 = strtok(NULL, " ");
                char *val2 = strtok(NULL, " ");
                if (strtok(NULL, " ") != NULL) {
                    fprintf(stderr, "invalid syntax. skipping file\n");
                    this->vertices.clear();
                    this->indices.clear();
                    return;
                }

                this->vertices.push_back(Vertex());
                this->vertices.rbegin()->coord[0] = atof(val0) / 3;
                this->vertices.rbegin()->coord[1] = atof(val1) / 3;
                this->vertices.rbegin()->coord[2] = atof(val2) / 3;
            } else {
                obj_file.seekg(cursor, std::ios_base::beg);
                break;
            }

            cursor = obj_file.tellg();
        }

        // parse normals
        size_t normal_cursor = 1;
        while (obj_file.getline(buffer, buffer_size)) {
            char *data_entry_type = strtok(buffer, " ");
            if (strcmp(data_entry_type, "vn") == 0) {
                char *val0 = strtok(NULL, " ");
                char *val1 = strtok(NULL, " ");
                char *val2 = strtok(NULL, " ");
                if (strtok(NULL, " ") != NULL) {
                    fprintf(stderr, "invalid syntax. skipping file\n");
                    this->vertices.clear();
                    this->indices.clear();
                    return;
                }

                this->vertices[normal_cursor].normal[0] = atof(val0);
                this->vertices[normal_cursor].normal[1] = atof(val1);
                this->vertices[normal_cursor].normal[2] = atof(val2);
            } else {
                obj_file.seekg(cursor, std::ios_base::beg);
                break;
            }

            cursor = obj_file.tellg();
            normal_cursor++;
        }
        DEBUG_assert(normal_cursor == this->vertices.size());

        // parse faces
        while (obj_file.getline(buffer, buffer_size)) {
            char *data_entry_type = strtok(buffer, " ");
            if (strcmp(data_entry_type, "f") == 0) {
                char *val0 = strtok(NULL, " ");
                char *val1 = strtok(NULL, " ");
                char *val2 = strtok(NULL, " ");
                if (strtok(NULL, " ") != NULL) {
                    fprintf(stderr, "invalid syntax. skipping file\n");
                    this->vertices.clear();
                    this->indices.clear();
                    return;
                }

                // check for '/'
                char *slash;
                GLuint v_index;
                GLuint vn_index;

                slash = strtok(val0, "/");
                DEBUG_assert(slash != NULL);
                v_index = atoi(val0);
                val0 = strtok(NULL, "/");
                DEBUG_assert(val0 != NULL);
                vn_index = atoi(val0);
                DEBUG_assert(v_index == vn_index);
                this->indices.push_back(v_index);

                slash = strtok(val1, "/");
                DEBUG_assert(slash != NULL);
                v_index = atoi(val1);
                val1 = strtok(NULL, "/");
                DEBUG_assert(val1 != NULL);
                vn_index = atoi(val1);
                DEBUG_assert(v_index == vn_index);
                this->indices.push_back(v_index);

                slash = strtok(val2, "/");
                DEBUG_assert(slash != NULL);
                v_index = atoi(val2);
                val2 = strtok(NULL, "/");
                DEBUG_assert(val2 != NULL);
                vn_index = atoi(val2);
                DEBUG_assert(v_index == vn_index);
                this->indices.push_back(v_index);
            } else {
                obj_file.seekg(cursor, std::ios_base::beg);
                break;
            }

            cursor = obj_file.tellg();
        }
    } else {
        fprintf(stderr, "can't open obj file: %s\n", filename);
    }
}

void Model::bind() {
    // set up vertex VBO
    glGenBuffers(1, &(this->vbo_vertex_id));
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertex_id);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * this->vertices.size(),
        this->vertices.data(),
        GL_STATIC_DRAW);
    // set up index VBO
    glGenBuffers(1, &(this->vbo_index_id));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo_index_id);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * this->indices.size(),
        this->indices.data(),
        GL_STATIC_DRAW);
}


void Model::setAmbient(
    const GLfloat r,
    const GLfloat g,
    const GLfloat b,
    const GLfloat a)
{
    this->material.ambient[0] = r;
    this->material.ambient[1] = g;
    this->material.ambient[2] = b;
    this->material.ambient[3] = a;
}

void Model::setDiffuse(
    const GLfloat r,
    const GLfloat g,
    const GLfloat b,
    const GLfloat a)
{
    this->material.diffuse[0] = r;
    this->material.diffuse[1] = g;
    this->material.diffuse[2] = b;
    this->material.diffuse[3] = a;
}

void Model::setSpecular(
    const GLfloat r,
    const GLfloat g,
    const GLfloat b,
    const GLfloat a)
{
    this->material.specular[0] = r;
    this->material.specular[1] = g;
    this->material.specular[2] = b;
    this->material.specular[3] = a;
}

void Model::setEmission(
    const GLfloat r,
    const GLfloat g,
    const GLfloat b,
    const GLfloat a)
{
    this->material.emission[0] = r;
    this->material.emission[1] = g;
    this->material.emission[2] = b;
    this->material.emission[3] = a;
}

void Model::setShininess(const GLfloat shininess) {
    this->material.shininess = shininess;
}
