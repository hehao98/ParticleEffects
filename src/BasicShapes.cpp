/*
 * Created by He Hao in 2018/5/2
 */

#include "BasicShapes.h"
#include "GL_Constants.h"

#include <glad/glad.h>
#include <json.hpp>

static unsigned int quadVAO, quadVBO;

static void initQuadVaoAndVbo()
{
    // init only once
    static bool flag = false;
    if (flag) return;
    flag = true;

    // positions
    glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
    glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);
    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);
    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);
    float quadVertices[] = {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    // configure plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VertexAttribLocations::vPos);
    glVertexAttribPointer(VertexAttribLocations::vPos, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(VertexAttribLocations::vNormal);
    glVertexAttribPointer(VertexAttribLocations::vNormal, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(VertexAttribLocations::vTexCoord);
    glVertexAttribPointer(VertexAttribLocations::vTexCoord, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(VertexAttribLocations::vTangent);
    glVertexAttribPointer(VertexAttribLocations::vTangent, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(VertexAttribLocations::vBitangent);
    glVertexAttribPointer(VertexAttribLocations::vBitangent, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
}

TexturedQuad::TexturedQuad(const char *jsonFile)
    : PbrGameObject(jsonFile) 
{
    initQuadVaoAndVbo();
    vao = quadVAO;
    vbo = quadVBO;
}

void TexturedQuad::render(const glm::mat4 &vp, Camera &camera)
{
    shader.use();

    glm::mat3 normalMatrix = glm::mat3(inverseTranspose(transform));
    
    // Vertex shader data
    shader.setMat4("vp", vp);
    shader.setMat4("model", transform);
    shader.setMat3("normalMatrix", normalMatrix); 

    // Fragment shader data
    shader.setInt("albedoIsSRGB", albedoIsSRGB);
    shader.setInt("albedoMap", TextureChannel::albedo);
    albedo.useTextureUnit(TextureChannel::albedo);  

    shader.setInt("normalIsSRGB", normalIsSRGB);
    shader.setInt("normalMap", TextureChannel::normal);
    normal.useTextureUnit(TextureChannel::normal);

    shader.setInt("metallicSmoothnessIsSRGB", metallicSmoothnessIsSRGB);
    shader.setInt("metallicSmoothnessMap", TextureChannel::metallicSmoothness);
    metallicSmoothness.useTextureUnit(TextureChannel::metallicSmoothness);
    shader.setFloat("smoothnessFactor", smoothnessFactor);

    shader.setInt("aoMap", TextureChannel::ao);
    ao.useTextureUnit(TextureChannel::ao);
    shader.setBool("hasAO", hasAO);

    shader.setBool("hasHeightMap", hasHeightMap);
    shader.setInt("heightMap", TextureChannel::height);
    heightMap.useTextureUnit(TextureChannel::height);

    shader.setInt("irradianceMap", TextureChannel::irradiance);
    glActiveTexture(GL_TEXTURE0 + TextureChannel::irradiance);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance);

    shader.setInt("prefilterMap", TextureChannel::prefilter);
    glActiveTexture(GL_TEXTURE0 + TextureChannel::prefilter);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter);

    shader.setInt("brdfLUT", TextureChannel::brdfLUT);
    glActiveTexture(GL_TEXTURE0 + TextureChannel::brdfLUT);
    glBindTexture(GL_TEXTURE_2D, brdfLUT);

    shader.setVec3("camPos", camera.Position);

    // The 0th component is directional light
    shader.setInt("lightCount", lightCount);
    for (int i = 0; i < lightCount; ++i) {
        shader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
        shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}