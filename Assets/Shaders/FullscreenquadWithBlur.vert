#version 430

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texC;

out vec2 out_texc;

void main () 
{
	out_texc = in_texC;
	gl_Position = vec4(in_pos, 1.0);
}
