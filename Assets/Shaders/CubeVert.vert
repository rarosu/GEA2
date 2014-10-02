#version 430

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texc;

out vec3 out_norm;
out vec2 out_texc;

void main () 
{
	gl_Position = in_pos;
	out_texc = in_texc;
	out_norm = in_norm;
}
