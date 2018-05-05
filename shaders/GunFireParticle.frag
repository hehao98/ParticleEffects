#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D sprite;

void main()
{
    FragColor = texture(sprite, TexCoord);
    if (FragColor.x == 0.0 && FragColor.y == 0.0 && FragColor.z == 0.0)
        discard;
    if (FragColor.a <= 0.02) discard;
}