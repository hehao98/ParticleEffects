#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

in float ElapsedTime[1];

out vec2 TexCoord;

uniform mat4 vp;

uniform vec3 camPos;

uniform int spriteRow;
uniform int spriteColumn;
uniform float lifetime;

void main()
{
    float unitRow = 1.0 / spriteRow;
    float unitColumn = 1.0 / spriteColumn;
    int seq = int((ElapsedTime[0]/lifetime) * (spriteColumn*spriteRow));
    int row = spriteRow - seq / spriteColumn;
    int column = spriteColumn - seq + row * spriteColumn;


    vec3 pos    = gl_in[0].gl_Position.xyz;
    vec3 up     = vec3(0.0, 1.0, 0.0);
    vec3 camDir = normalize(camPos - pos);
    vec3 right  = cross(camDir, up);

    pos -= right * 0.5;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2(column * unitColumn, row * unitRow);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2(column * unitColumn, (row+1) * unitRow);
    EmitVertex();

    pos.y -= 1.0;
    pos += right;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2((column+1) * unitColumn, row * unitRow);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2((column+1) * unitColumn, (row+1) * unitRow);
    EmitVertex();
    EndPrimitive();
}