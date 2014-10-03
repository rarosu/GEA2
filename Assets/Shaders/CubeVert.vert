#version 430

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec3 in_norm;

out vec3 out_norm;
out vec2 out_texc;
out int out_materialIndex;
out float out_shade;

const vec3 g_dirToSunWorld = vec3(0.7, 1.0, 0.8);

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewProjectionMatrix;
};

void main () 
{
	vec4 position = vec4(in_pos.xyz, 1.0);
	out_texc = vec2(0, 0);
	out_norm = in_norm;
	out_materialIndex = int(in_pos.w);
	out_shade = clamp(max(dot(in_norm, normalize(g_dirToSunWorld)), 0.0) + 0.3, 0.0, 1.0);
	gl_Position = viewProjectionMatrix * position;
}
