#version 430

in vec2 out_texc;

out vec4 color;

layout(binding = 0) uniform sampler2D diffuseTex;
layout(binding = 1) uniform sampler2D normalTex;
layout(binding = 2) uniform sampler2D depthTex;

void main()
{
//OTHER
	//float depth = texture(depthTex, out_texc).r;
	 //depth = LinearizeDepth(depth)*3;
	// color = vec4(depth, depth, depth, 1)/10;
	//color = texture(normalTex, out_texc);

	
	color = texture(diffuseTex, out_texc);
}
