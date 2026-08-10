#version 330 core
uniform vec3 uObjectColor;

// Lighting
uniform vec3 uLightColor;
uniform float uAmbientStrength;
uniform float uSpecularStrength;

in vec3 FragPos;
in vec3 VertexColor;
in vec3 Normal;
in vec3 LightPos;

out vec4 FragColor;

void main()
{
    // AmbientStrength
    vec3 Ambient = uAmbientStrength * uLightColor;
    
    // Diffuse
    vec3 Norm = normalize(Normal);
    vec3 LightDir = normalize(LightPos - FragPos);
    float Diff = max(dot(Norm, LightDir), 0.0f);
    vec3 Diffuse = Diff * uLightColor;
    
    // Specular
    vec3 ViewDir = normalize(-FragPos);
    vec3 ReflectDir = reflect(-LightDir, Norm);
    float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0f), 32);
    vec3 Specular = uSpecularStrength * Spec * uLightColor;
    
    vec3 Lighting = (Ambient + Diffuse + Specular);
    vec3 Result = Lighting * uObjectColor;
    
    FragColor = vec4(Result, 1.0f);
}