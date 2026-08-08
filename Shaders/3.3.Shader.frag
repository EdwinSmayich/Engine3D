#version 330 core
uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform float uAmbientStrength;

in vec3 VertexColor;
out vec4 FragColor;

void main()
{
    vec3 Ambient = uAmbientStrength * uLightColor;
    vec3 Result = uAmbientStrength * uObjectColor;
    
    FragColor = vec4(Result, 1.0f);
}