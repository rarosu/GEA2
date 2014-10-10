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
	vec3 camPos;
};

layout(binding = 1, std140) uniform Time
{
	float time;
};
out vec2 out_texc;
out vec3 out_posV;
out vec3 out_posW;
out vec3 out_normV;

void main () 
{
	out_texc = in_texC;
	out_posW = in_pos.xyz;
	vec4 posV = viewMatrix * vec4(in_pos, 1.0);
	out_posV = posV.xyz;
	vec4 normV =  viewMatrix * vec4(in_norm, 0);
	out_normV = normV.xyz;
	gl_Position = projMatrix * posV;
}
