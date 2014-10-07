#version 430

in vec2 out_texc;

out vec4 color;

layout(binding = 0) uniform sampler2D diffuseTex;
layout(binding = 1) uniform sampler2D normalTex;
layout(binding = 2) uniform sampler2D depthTex;
layout(binding = 3) uniform sampler2D ssao;

const int blurSize = 16;

void main()
{
//OTHER
	//float depth = texture(depthTex, out_texc).r;
	 //depth = LinearizeDepth(depth)*3;
	// color = vec4(depth, depth, depth, 1)/10;
	//color = texture(normalTex, out_texc);

	vec2 texelSize = 1.0 / vec2(textureSize(ssao, 0));
	float result = 0.0;
	vec2 hlim = vec2(float(-blurSize) * 0.5 + 0.5);
	for (int i = 0; i < blurSize; ++i) 
	{
		for (int j = 0; j < blurSize; ++j) 
		{
			vec2 offset = (hlim + vec2(float(i), float(j))) * texelSize;
			result += texture(ssao, out_texc + offset).r;
		}
	}
 
	result = result / float(blurSize * blurSize);

	color = texture(diffuseTex, out_texc) * texture(ssao, out_texc).r;//* result;
}
