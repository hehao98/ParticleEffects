#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.h"

#include <iostream>

Texture::Texture(const char *imagePath)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // Set default texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 3) { // RGB
            std::cout << "RGB Image " << imagePath << " Loaded" << std::endl;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) { // RGBA
            std::cout << "RGBA Image " << imagePath << " Loaded" << std::endl;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 1) { // Gray
            std::cout << "Gray Image " << imagePath << " Loaded" << std::endl;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height,
                         0, GL_RED, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Warning: Unhandled Texture Color channel: " << imagePath << std::endl;
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture: " << imagePath << std::endl;
    }
    stbi_set_flip_vertically_on_load(false);
    stbi_image_free(data);
}

// activeTextureUnit should be a texture unit ID between 0 and 15
void Texture::useTextureUnit(int activeTextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}