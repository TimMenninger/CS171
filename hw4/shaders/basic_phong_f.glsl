/*----------------------------------------------------------------------------\

basic_phong_f.glsl

Fragment shader GLSL source.  This file process the facets of the image
being displayed.

Revisions: 10/??/2016 - CS171: Created skeleton
           11/15/2016 - Tim Menninger: Implemented Phong shader

-----------------------------------------------------------------------------*/

// Used for determining light intensity
varying vec3 normal;
varying vec3 vertex;

void main (void) {
    // Extract shininess now so we don't have to keep finding it in iteration
    float shininess = gl_FrontMaterial.shininess;

    // Keep a running sum of the ambient, diffuse and specular components
    vec4 ambientSum  = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 diffuseSum  = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 specularSum = vec4(0.0, 0.0, 0.0, 0.0);

    // Compute the contribution of each light
    for (int l = 0; l < gl_MaxLights; ++l) {
        // Extract light components for readability
        vec4 lightAmbient  = gl_FrontLightProduct[l].ambient;
        vec4 lightDiffuse  = gl_FrontLightProduct[l].diffuse;
        vec4 lightSpecular = gl_FrontLightProduct[l].specular;

        // Extract light xyz position
        vec3 lightPos = gl_LightSource[l].position.xyz;

        // NOTE:
        // Compute attenuation.  In looking up documentation, I found a
        // constant, linear and quadratic coefficient.  I assume these
        // coefficients correspond to distance, so this is slightly different
        // than the HW2 attenuation calculation.  I also couldn't find a
        // color attached to gl_LightSource, so I assume that's somehow
        // included in the components.  Thus, I multiply each component by
        // the attenuation for each light source.

        // Distance from light source to vertex
        float d = distance(vertex, lightPos);
        vec3 k = vec3(gl_LightSource[l].constantAttenuation,
                      gl_LightSource[l].linearAttenuation,
                      gl_LightSource[l].quadraticAttenuation);
        float attenuation = 1.0 / (k[2]*d*d + k[1]*d + k[0]);

        // Normalized vector in direction of light source
        vec3 lDir = normalize(lightPos - vertex);
        // Normalized vector in direction of eye
        vec3 eDir = normalize(-vertex);

        // Calculate ambience given the lights
        ambientSum += attenuation * lightAmbient;

        // Calculate diffuse given the lights
        diffuseSum += attenuation * lightDiffuse * max(0.0, dot(normal, lDir));

        // Calculate specular given the lights
        float spec = max(0.0, dot(normalize(eDir + lDir), normal));
        specularSum += attenuation * lightSpecular * pow(spec, 0.3*shininess);
    }

    // Compute the final light intensity, and bound it between 0 and 1
    vec4 colorSum = ambientSum + diffuseSum + specularSum;
    for (int i = 0; i < 4; ++i)
        colorSum[i] = clamp(colorSum[i], 0.0, 1.0);

    // write Total Color:
    gl_FragColor = colorSum;
}
