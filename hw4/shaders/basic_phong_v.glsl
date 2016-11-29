/*----------------------------------------------------------------------------\

basic_phong_v.glsl

Vertex shader GLSL source.  This file process the vertices of the image
being displayed.

Revisions: 10/??/2016 - CS171: Created skeleton
           11/15/2016 - Tim Menninger: Implemented Phong shader

-----------------------------------------------------------------------------*/

// Parameters that we use in iteration to determine light color and intensity
varying vec3 normal;
varying vec3 vertex;

// Main loop
void main(void) {
    // Transform our vertex
    vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    // Normalize normal vector
    normal = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
