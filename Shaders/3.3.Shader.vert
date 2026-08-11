#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

// Lighting
uniform mat3 uNormalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal =  uNormalMatrix * aNormal;
    
    gl_Position = uProjection * uView * vec4(FragPos, 1.0f);
    
    TexCoords = aTexCoords;
}