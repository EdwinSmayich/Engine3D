#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 OurColorFromPos;
out vec2 TexCoord;
uniform float OffsetX;

void main()
{
  	gl_Position = vec4(aPos.x + OffsetX, aPos.y, aPos.z, 1.0);
	OurColorFromPos = aPos;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}