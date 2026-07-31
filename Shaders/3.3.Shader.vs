#version 330 core
uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 TexCoord;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}