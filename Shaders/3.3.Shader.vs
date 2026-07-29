#version 330 core
uniform mat4 uMVP;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 TexCoord;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}