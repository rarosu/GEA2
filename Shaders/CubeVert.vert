#version 430

layout(location = 0) in uvec4 in_pos;
layout(location = 1) in vec2 in_texc;
layout(location = 2) in vec3 in_norm;

out vec2 out_texc;
out vec3 out_norm;

void main () 
{
	gl_Position = vec4 (in_pos, 1.0);
	out_texc = in_texc;
	out_norm = in_norm;
}