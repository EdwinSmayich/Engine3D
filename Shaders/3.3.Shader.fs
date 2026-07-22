#version 330 core
uniform sampler2D uTexture;
uniform vec4 OurColor;

//in vec3 VertexColor;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    FragColor = texture(uTexture, TexCoord) * OurColor;
}