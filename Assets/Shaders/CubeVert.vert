#version 430

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texC;

out vec3 out_posV;
out vec3 out_normV;
out vec2 out_texc;
out flat float out_shade;

const vec3 g_dirToSunWorld = vec3(0.7, 1.0, 0.8);

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 invviewMatrix;
	mat4 invprojMatrix;
};

void main () 
{
	vec4 position = viewMatrix * vec4(in_pos.xyz, 1.0);
	out_texc = in_texC;
	out_normV = vec3(viewMatrix * vec4(in_norm, 0));
	out_shade = clamp(max(dot(in_norm, normalize(g_dirToSunWorld)), 0.0) + 0.3, 0.0, 1.0);
	out_posV = position.xyz;
	gl_Position =  projMatrix * position;
}
