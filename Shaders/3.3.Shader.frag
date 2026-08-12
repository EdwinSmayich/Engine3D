#version 330 core
struct Material
{
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
};

struct Light
{
    vec3 Position;
    vec3 Color;
    //    vec3 Ambient;
    //    vec3 Diffuse;
    //    vec3 Specular;
    //    float AmbientStrength;
    //    float DiffuseStrength;
    //    float SpecularStrength;
};

#define MAX_LIGHTS 16
uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;
uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uSpecularStrength;
vec3 CalculateLight(Light InLight, vec3 InNorm, vec3 InViewDir);

uniform vec3 uViewPos;
uniform Material uMaterial;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 Norm = normalize(Normal);
    vec3 ViewDir = normalize(uViewPos - FragPos);

    vec3 Result = vec3(0.0f);
    for (int i = 0; i < uLightCount; ++i)
    {
        Result += CalculateLight(uLights[i], Norm, ViewDir);
    }

    FragColor = vec4(Result, 1.0f);
}

vec3 CalculateLight(Light InLight, vec3 InNorm, vec3 InViewDir)
{
    // Ambient
    vec3 Ambient = uAmbientStrength * (InLight.Color * vec3(texture(uMaterial.Diffuse, TexCoords)));

    // Diffuse
    vec3 LightDir = normalize(InLight.Position - FragPos);
    float Diff = max(dot(InNorm, LightDir), 0.0f);
    vec3 Diffuse = uDiffuseStrength * (InLight.Color * (Diff * vec3(texture(uMaterial.Diffuse, TexCoords))));

    // Specular and Shininess
    vec3 ReflectDir = reflect(-LightDir, InNorm);
    float Spec = pow(max(dot(InViewDir, ReflectDir), 0.0f), uMaterial.Shininess);
    vec3 Specular = uSpecularStrength * (InLight.Color * (Spec * vec3(texture(uMaterial.Specular, TexCoords))));

    // Return final color
    return (Ambient + Diffuse + Specular);
}