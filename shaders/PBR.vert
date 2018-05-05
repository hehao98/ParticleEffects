#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec4 FragPos; // world space
out vec3 Normal;  // world space
out vec2 TexCoord;
out mat3 TBNMatrix;
out vec3 TangentCamPos;
out vec3 TangentLightPos;
out vec3 TangentFragPos;

uniform mat4 vp;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform vec3 camPos;

void main()
{
    gl_Position = vp * model * vec4(vPos, 1.0);

    // Pass frag position and normal in world space
    FragPos = model * vec4(vPos, 1.0);
    Normal = normalMatrix * vNormal;

    TexCoord = vTexCoord;

    // Trasnform frag position, camera position to tangent space
    // for normal mapping
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = transpose(mat3(T, B, N));
    TBNMatrix = TBN;
    TangentCamPos  = TBN * camPos;
    TangentFragPos = TBN * vec3(model * vec4(vPos, 0.0));
}