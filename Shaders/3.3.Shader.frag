#version 330 core
struct Material
{
    sampler2D Diffuse;
    sampler2D Specular;
    sampler2D Emission;
    float Shininess;
};

struct Light
{
    vec3 Position;
    vec3 Direction;
    vec3 Color;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    // Attenuation
    float Constant;
    float Linear;
    float Quadratic;

    float InnerCutoff;
    float OuterCutoff;

    // Direction / Point / Spot
    int LightingType;
};

#define MAX_LIGHTS 16
uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;

vec3 CalculateDirLight(Light InLight, vec3 InNorm, vec3 InFragPos, vec3 InViewDir);
vec3 CalculatePointLight(Light InLight, vec3 InNorm, vec3 InFragPos, vec3 InViewDir);
vec3 CalculateSpotLight(Light InLight, vec3 InNorm, vec3 InFragPos, vec3 InViewDir);

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
        switch (uLights[i].LightingType)
        {
            case 0:
                {
                    Result += CalculateDirLight(uLights[i], Norm, FragPos, ViewDir);
                    break;
                }
            case 1:
                {
                    Result += CalculatePointLight(uLights[i], Norm, FragPos, ViewDir);
                    break;
                }
            case 2:
                {
                    Result += CalculateSpotLight(uLights[i], Norm, FragPos, ViewDir);
                    break;
                }
        }
    }

    FragColor = vec4(Result, 1.0f);
}

vec3 CalculateDirLight(Light InLight, vec3 InNorm, vec3 InFragPos, vec3 InViewDir)
{
    // Ambient
    vec3 Ambient = InLight.Color * (InLight.Ambient * texture(uMaterial.Diffuse, TexCoords).rgb);

    // Diffuse
    vec3 LightDir = normalize(-InLight.Direction);
    float Diff = max(dot(InNorm, LightDir), 0.0f);
    vec3 Diffuse = InLight.Diffuse * InLight.Color * (Diff * texture(uMaterial.Diffuse, TexCoords).rgb);

    // Specular and Shininess
    vec3 ReflectDir = reflect(-LightDir, InNorm);
    float Spec = pow(max(dot(InViewDir, ReflectDir), 0.0f), uMaterial.Shininess);
    vec3 Specular = InLight.Specular * InLight.Color * (Spec * texture(uMaterial.Specular, TexCoords).rgb);

    // Return final color
    return (Ambient + Diffuse + Specular);
}

vec3 CalculatePointLight(Light InLight, vec3 InNorm, vec3 InFragPos, vec3 InViewDir)
{
    // Ambient
    vec3 Ambient = InLight.Color * (InLight.Ambient * texture(uMaterial.Diffuse, TexCoords).rgb);

    // Diffuse
    vec3 LightDir = normalize(InLight.Position - InFragPos);
    float Diff = max(dot(InNorm, LightDir), 0.0f);
    vec3 Diffuse = InLight.Color * (Diff * texture(uMaterial.Diffuse, TexCoords).rgb);

    // Specular and Shininess
    vec3 ReflectDir = reflect(-LightDir, InNorm);
    float Spec = pow(max(dot(InViewDir, ReflectDir), 0.0f), uMaterial.Shininess);
    vec3 Specular = InLight.Color * (Spec * texture(uMaterial.Specular, TexCoords).rgb);

    // Attenuation
    float Distance = length(InLight.Position - InFragPos);
    float Attenuation = 1.0f /
    (InLight.Constant + InLight.Linear * Distance +
    InLight.Quadratic * (Distance * Distance));

    Ambient *= Attenuation;
    Diffuse *= Attenuation;
    Specular *= Attenuation;

    // Return final color
    return (Ambient + Diffuse + Specular);
}

vec3 CalculateSpotLight(Light InLight, vec3 InNorm, vec3 InFragPos, vec3 InViewDir)
{
    // Ambient
    vec3 Ambient = InLight.Color * InLight.Ambient * texture(uMaterial.Diffuse, TexCoords).rgb;

    // Diffuse
    vec3 LightDir = normalize(InLight.Position - InFragPos);
    float Diff = max(dot(InNorm, LightDir), 0.0f);
    vec3 Diffuse = InLight.Diffuse * InLight.Color * (Diff * texture(uMaterial.Diffuse, TexCoords).rgb);

    // Specular and Shininess
    vec3 ReflectDir = reflect(-LightDir, InNorm);
    float Spec = pow(max(dot(InViewDir, ReflectDir), 0.0f), uMaterial.Shininess);
    vec3 Specular = InLight.Specular * InLight.Color * (Spec * texture(uMaterial.Specular, TexCoords).rgb);

    // Attenuation
    float Distance = length(InLight.Position - InFragPos);
    float Attenuation = 1.0f /
    (InLight.Constant + InLight.Linear * Distance +
    InLight.Quadratic * (Distance * Distance));

    Ambient *= Attenuation;
    Diffuse *= Attenuation;
    Specular *= Attenuation;

    float Theta = dot(LightDir, normalize(-InLight.Direction));
    float Epsilon = InLight.InnerCutoff - InLight.OuterCutoff;
    float Intensity = clamp((Theta - InLight.OuterCutoff) / Epsilon, 0.0f, 1.0f);

    Diffuse *= Intensity;
    Specular *= Intensity;

    // Return final color
    return (Ambient + Diffuse + Specular);
}