#version 430

in vec2 out_texc;

out vec3 color;

layout(binding = 0) uniform sampler2D diffuseTex;
layout(binding = 1) uniform sampler2D normalTex;
layout(binding = 2) uniform sampler2D depthTex;
layout(binding = 3) uniform sampler2D g_Noise;

layout(binding = 0, std140) uniform PerFrame
{
	mat4 viewMatrix;
	mat4 projMatrix;
	mat4 invviewMatrix;
	mat4 invprojMatrix;
};

layout(binding = 1, std140) uniform SampleKern
{
	vec4 uSampleKernel[16]; 	
};

const float uRadius = 1.5;
const float uPower = 2.0;
const int uSampleKernelSize = 16;

vec3 GetVSPositionFromDepth(vec2 sampleCoord)
{
	float z = texture(depthTex, sampleCoord).r;

	z = z * 2 - 1;

	float x = sampleCoord.x * 2 - 1;
	float y = sampleCoord.y * 2 - 1;

	vec4 vProjectedPos = vec4(x, y, z, 1.0);
	vec4 sPos = invprojMatrix * vProjectedPos;

	return (sPos.xyz / sPos.w);
}


void main()
{
//SSAO

	vec3 origin = GetVSPositionFromDepth(out_texc);
	vec3 normal = texture(normalTex, out_texc).xyz;
	vec3 rvec = texture(g_Noise, out_texc * textureSize(depthTex, 0) * 0.25).xyz * 2.0 - 1.0;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	float sampleDepth = 0;
	vec3 samp;
	float occlusion = 0.0;
	for (int i = 0; i < uSampleKernelSize; ++i) 
	{
		// get sample position:
		samp = tbn * uSampleKernel[i].xyz;
		samp = samp * uRadius + origin;

		// project sample position:
		vec4 offset = vec4(samp, 1.0);
		offset = projMatrix * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		// get sample depth:
		sampleDepth = GetVSPositionFromDepth(offset.xy).z;

		// range check & accumulate:
		float rangeCheck = abs(origin.z - sampleDepth) < uRadius ? 1.0 : 0.0;
		occlusion += (sampleDepth > samp.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1 - (occlusion / float(uSampleKernelSize));
	occlusion = pow(occlusion, uPower);
	if(occlusion > 0.5)
		occlusion = 1;
	color = vec3(occlusion, occlusion, occlusion);
}