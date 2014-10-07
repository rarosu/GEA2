#version 430

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texC;

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 invviewMatrix;
	mat4 invprojMatrix;
};

layout(binding = 1, std140) uniform SampleKern
{
	vec4 uSampleKernel[16]; 	
};

out vec2 out_texc;

void main () 
{
	out_texc = in_texC;
	gl_Position = vec4(in_pos, 1.0);
}
