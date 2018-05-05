//
// Simple wrapper class for OpenGL Texture
// Created by Hao He on 18-1-24.
//

#ifndef PROJECT_TEXTURE_H
#define PROJECT_TEXTURE_H

#include <glad/glad.h>

class Texture
{
public:
    unsigned int ID;

    Texture() { ID = 0; }
    explicit Texture(const char *imagePath);

    // activeTextureUnit should be a texture unit ID between 0 and 15
    void useTextureUnit(int activeTextureUnit = 0);
};


#endif //PROJECT_TEXTURE_H
