#version 430

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texc;

out vec3 out_norm;
out vec2 out_texc;

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewProjectionMatrix;
};

void main () 
{
	vec4 position = vec4(in_pos.xyz, 1.0);
	gl_Position = viewProjectionMatrix * position;
	out_texc = in_texc;
	out_norm = in_norm;
}
