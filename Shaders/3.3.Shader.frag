#version 330 core
struct Material
{
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

struct Light
{
    vec3 Position;
    vec3 LightColor;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float AmbientStrength;
    float SpecularStrength;
};

uniform vec3 uViewPos;

// Material
uniform Material uMaterial;
uniform Light uLight;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    // Ambient
    vec3 Ambient = uLight.LightColor * (uLight.AmbientStrength * uLight.Ambient * uMaterial.Ambient);
    
    // Diffuse
    vec3 Norm = normalize(Normal);
    vec3 LightDir = normalize(uLight.Position - FragPos);
    float Diff = max(dot(Norm, LightDir), 0.0f);
    vec3 Diffuse = uLight.LightColor * (uLight.Diffuse * Diff * uMaterial.Diffuse);
    
    // Specular and Shininess
    vec3 ViewDir = normalize(uViewPos - FragPos);
    vec3 ReflectDir = reflect(-LightDir, Norm);
    float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0f), uMaterial.Shininess);
    vec3 Specular = uLight.LightColor * (uLight.SpecularStrength * uLight.Specular * Spec * uMaterial.Specular);
    
    // Final color
    vec3 Result = (Ambient + Diffuse + Specular);
    FragColor = vec4(Result, 1.0f);
}