#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

out vec2 TexCoord;

uniform mat4 vp;

uniform vec3 camPos;

void main()
{
    vec3 pos    = gl_in[0].gl_Position.xyz;
    vec3 up     = vec3(0.0, 1.0, 0.0);
    vec3 camDir = normalize(camPos - pos);
    vec3 right  = cross(camDir, up);

    pos -= right * 0.5;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    pos.y -= 1.0;
    pos += right;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = vp * vec4(pos, 1.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}