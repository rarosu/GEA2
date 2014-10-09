#version 430

in vec2 out_texc;

out vec4 color;

layout(binding = 0) uniform sampler2D diffuseTex;

void main()
{	
	color = texture(diffuseTex, out_texc);
}
