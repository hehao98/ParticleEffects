#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out VS_OUT {
    vec4 fragPos;
    vec3 normal;
    vec2 texCoord;
} vs_out;

uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{
    gl_Position = mvp * vec4(vPos, 1.0);
    vs_out.fragPos = gl_Position;
    vs_out.normal = normalMatrix * vNormal;
    vs_out.texCoord = vTexCoord;
}