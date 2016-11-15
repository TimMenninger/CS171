// #define DEBUG_PRINT
#define DEBUG_ASSERT

#include "renderer.hpp"

#include "model.hpp"

void renderModel(Model &model) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, model.getMaterial().ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, model.getMaterial().diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, model.getMaterial().specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, model.getMaterial().emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, model.getMaterial().shininess);

    glPushMatrix();
    // bind buffer
    glBindBuffer(GL_ARRAY_BUFFER, model.getVBOVertexID());
    glVertexPointer(
        3,              // size
        GL_FLOAT,       // type
        sizeof(Vertex), // stride
        0);             // pointer offset
                        // ^ pointer offset instead of pointer for VBOs
    glNormalPointer(
        GL_FLOAT,                           // type
        sizeof(Vertex),                     // stride
        (GLvoid*) (sizeof(GLfloat) * 3));   // pointer offset
                                            // assumes padding bytes = 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.getVBOIndexID());
    glIndexPointer(
        GL_UNSIGNED_INT,        // type
        0,                      // stride
        0);                     // pointer offset

    // actually draw
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    glDrawElements(
        GL_TRIANGLES,                   // type
        model.getIndices().size(),     // size
        GL_UNSIGNED_INT,                // type of index
        (GLuint*) 0);                   // pointer

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glPopMatrix();
}
