/*
 * A class that automatically load an HDR enviroment map
 * which can be used in Image Based Lighting
 */

#ifndef ENVIRONMENTMAP_H
#define ENVIRONMENTMAP_H

#include <iostream>

#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <glad/glad.h>

#include "Camera.h"
#include "Shader.h"

class EnvironmentMap
{
public:
    unsigned int hdrTexture;
 
    // VAO and VBO of a cube
    unsigned int vao, vbo;

    // The shader used to render skybox
    Shader shader;

    Shader prefilterShader;

    Shader brdfShader;

    // FBO and RBO used to generate our own cubemap texture
    unsigned int captureFBO, captureRBO;

    unsigned int envCubemap;

    unsigned int prefilterMap;

    // BRDF look up texture
    unsigned int brdfLUT;

    explicit EnvironmentMap(const char *texturePath, int cubeMapRes = 512);

    void render(const glm::mat4 &view, const glm::mat4 &projection, Camera &camera);

    void renderSkybox(const glm::mat4 &view, const glm::mat4 &projection, Camera &camera);

    void renderSkybox(const glm::mat4 &view, const glm::mat4 &projection, Camera &camera, unsigned int cubemap);
};

class SphereSkybox
{
public:
    unsigned int hdrTexture;
 
    // VAO and VBO of a cube
    unsigned int vao, vbo;

    // The shader used to render skybox
    Shader shader;

    // FBO and RBO used to generate our own cubemap texture
    unsigned int captureFBO, captureRBO;

    unsigned int envCubemap;

    explicit SphereSkybox(const char *texturePath);

    void render(const glm::mat4 &view, const glm::mat4 &projection, Camera &camera);
};


#endif

