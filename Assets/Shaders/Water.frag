#version 430

in vec2 out_texc;

out vec4 color;

layout(binding = 0) uniform sampler2D diffuseTex;
layout(binding = 1) uniform sampler2D normalTex;

void main()
{
	//color = texture(normalTex, out_texc);
	color = texture(diffuseTex, out_texc);
}
