#version 430

in vec3 out_posV;
in vec3 out_normV;
in vec2 out_texc;
in flat float out_shade;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normals;
layout(location = 2) out vec3 vpos;

layout(binding = 0) uniform sampler2D diffuseTex;

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 invviewMatrix;
	mat4 invprojMatrix;
};

layout(binding = 1, std140) uniform PerObject
{
	mat4 worldMatrix;
};

void main()
{
	color = texture(diffuseTex, out_texc).xyz * out_shade;
	normals = out_normV;
	vpos = out_posV;
}
