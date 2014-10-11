#version 430

in vec2 out_texc;
in vec3 out_posV;
in vec3 out_posW;
in vec3 out_normV;

out vec3 color;

layout(binding = 0) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D posTex;
layout(binding = 3) uniform sampler2D diffuseTex;
//layout(binding = 4) uniform sampler2D normalMap;

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

float Fresnel(float p_dot, float p_fresnelBias, float p_fresnelPow)
{
	float facing = (1.0 - p_dot);
	return max(p_fresnelBias + (1.0 - p_fresnelBias) * pow(facing, p_fresnelPow), 0.0);
}

void main()
{
	////////////////////////////////////////////////////////////////////////////
	//Calculate transparent color and refraction - Need more accuracy when refracting using view space normals
	////////////////////////////////////////////////////////////////////////////

	mat3 TBN				= mat3(vec3(0, 0, -1), vec3(-1, 0, 0), out_normV);

	vec3 normal 			= texture(normalMap, out_texc * 32.0 + vec2(time*0.05, 0)).xyz;
	normal 					= normalize(TBN * normal);
	vec4 normalM 			= normalize(viewMatrix * vec4(normal.xyz, 0));
	vec2 screenTexCoord		= gl_FragCoord.xy / textureSize(diffuseTex, 0);
	ivec2 screenAbsCoord	= ivec2(gl_FragCoord.xy);
	ivec2 refractedUV		= clamp(screenAbsCoord + ivec2(vec2(normalM.x, normalM.z) * 15.0), ivec2(0), textureSize(diffuseTex, 0));
	float refractionDepth	= texelFetch(posTex, refractedUV, 0).z;

	vec3 refractionColor;
	//Check if refracted vector is hitting an object in the foreground
	if(refractionDepth < out_posV.z)
		refractionColor	= texelFetch(diffuseTex, refractedUV, 0).rgb;
	else
	{
		refractionColor	= texelFetch(diffuseTex, ivec2(gl_FragCoord.xy), 0).rgb;
		refractionDepth	= texelFetch(posTex, ivec2(gl_FragCoord.xy), 0).z;
	}

	vec3 diffuseColor = vec3(0, 0, 0.5);
	////////////////////////////////////////////////////////////////////////////
	//Outputs
	////////////////////////////////////////////////////////////////////////////
	vec3 result =  mix(refractionColor, diffuseColor, 0.5);

	float vdist = abs(refractionDepth - out_posV.z);
	float distFac = clamp(0.02*vdist, 0.0, 1.0);
	
	vec3 waterColor	= mix(result, vec3(0, 0.1, 0.4), distFac);

	float ndotl		= max(dot(normalize(camPos - out_posW), normalize(normal.xyz)), 0.0);
	float fresnel	= Fresnel(ndotl, 0.2, 6.0);

	result = mix(waterColor, vec3(0, 0.4, 1), fresnel);

	color = result;
}
