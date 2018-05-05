/*
 * OpenGL Related constants are defined in this file
 * to make code clearer and avoid magic numbers
 */ 

#ifndef GL_CONSTANTS_H
#define GL_CONSTANTS_H

enum VertexAttribLocations {
    vPos       = 0,
    vNormal    = 1,
    vTexCoord  = 2,
    vTangent   = 3,
    vBitangent = 4,
};

enum TextureChannel {
    albedo             = 0,
    normal             = 1,
    metallicSmoothness = 2,
    ao                 = 3,
    skybox             = 4,
    irradiance         = 5,
    prefilter          = 6,
    brdfLUT            = 7,
    height             = 8,
    sprite             = 9,
};

#endif