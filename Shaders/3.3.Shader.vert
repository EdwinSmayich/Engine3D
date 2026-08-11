#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

// Lighting
//uniform vec3 uLightPos;
uniform mat3 uNormalMatrix;

out vec3 FragPos;
out vec3 Normal;
//out vec3 LightPos;

void main()
{
    // Lighting lesson
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal =  uNormalMatrix * aNormal;
   // LightPos = vec3(uView * vec4(uLightPos, 1.0));
    
    gl_Position = uProjection * uView * vec4(FragPos, 1.0f);
}