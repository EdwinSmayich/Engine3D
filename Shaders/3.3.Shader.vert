#version 330 core
uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform vec3 uLightPos;
uniform mat3 uNormalMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 VertexColor;
out vec3 Normal;
out vec3 LightPos;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
    VertexColor = aColor;
    
    // Lighting lesson
    FragPos = vec3(uView * uModel * vec4(aPos, 1.0));
    Normal =  uNormalMatrix * aNormal;
    LightPos = vec3(uView * vec4(uLightPos, 1.0));
}