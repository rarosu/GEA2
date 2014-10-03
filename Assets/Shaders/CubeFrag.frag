#version 430

const vec4 g_colors[12] = vec4[12](
vec4(0.0, 0.0, 1.0, 1.0), // Blue/water 1
vec4(1.0, 1.0, 0.0, 1.0), // Ore 2
vec4(0.0, 0.9, 0.0, 1.0), // Leaves1 3
vec4(0.4, 0.2, 0.0, 1.0), // Trunk 4
vec4(1.0, 1.0, 0.4, 1.0), //Sand 5
vec4(0.6, 0.4, 0.2, 1.0), //Dirt 6 
vec4(0.7, 0.7, 0.2, 1.0), //Rock 7
vec4(0.4, 0.6, 0.2, 1.0), //Grass 8
vec4(0.1, 0.7, 0.2, 1.0), //leaf2 9
vec4(0.0, 0.8, 0.1, 1.0), //leaf3 10
vec4(0.3, 0.99, 0.2, 1.0), //leaf4 11
vec4(0.1, 0.6, 0.0, 1.0) //leaf5 12
);

in vec3 out_norm;
in vec2 out_texc;
in flat float out_shade;

out vec4 color;

layout(binding = 0) uniform sampler2D diffuseTex;

void main()
{
	color = texture(diffuseTex, out_texc) * out_shade;
}
