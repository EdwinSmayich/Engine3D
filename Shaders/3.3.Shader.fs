#version 330 core
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform float uMixValue;

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    FragColor = mix(texture(uTexture1, TexCoord), texture(uTexture2, TexCoord), uMixValue);
}