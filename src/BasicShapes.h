/*
 * Classes to render basic geometry shapes like sphere, box, plane, etc
 * 
 * Created by He Hao at 2018/5/2
 */

#ifndef BASIC_SHAPES_H
#define BASIC_SHAPES_H

#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"
#include "Shader.h"
#include "EnvironmentMap.h"

class TexturedQuad : public PbrGameObject
{
public:
    unsigned int vao, vbo;

    // Load from json configuration file
    explicit TexturedQuad(const char *jsonFile);

    void render(const glm::mat4 &vp, Camera &camera) override;
};


#endif