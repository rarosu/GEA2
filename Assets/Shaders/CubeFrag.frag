#version 430

in vec3 out_normV;
in vec2 out_texc;
in flat float out_shade;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normals;


layout(binding = 0) uniform sampler2D diffuseTex;

void main()
{
	color = texture(diffuseTex, out_texc).xyz * out_shade;
	normals = out_normV;
}
