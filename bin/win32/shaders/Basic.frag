#version 330 core

out vec4 fragColor;

in VS_OUT {
    vec4 fragPos;
    vec3 normal;
    vec2 texCoord;
} fs_in;

void main()
{
    fragColor = vec4(fs_in.normal.x, fs_in.normal.y, fs_in.normal.z, 1.0);
}