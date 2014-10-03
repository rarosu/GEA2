#version 430

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texC;

out vec3 out_norm;
out vec2 out_texc;
out float out_shade;

const vec3 g_dirToSunWorld = vec3(0.7, 1.0, 0.8);

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewProjectionMatrix;
};

layout(binding = 1, std140) uniform PerObject
{
	mat4 worldMatrix;
};

void main () 
{
	vec3 position = in_pos.xyz;
	out_texc = in_texC;
	out_norm = in_norm;
	out_shade = clamp(max(dot(in_norm, normalize(g_dirToSunWorld)), 0.0) + 0.3, 0.0, 1.0);
	gl_Position = viewProjectionMatrix * worldMatrix * vec4(position, 1.0);
}
