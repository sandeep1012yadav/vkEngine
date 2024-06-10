#version 450

layout (set = 1, binding = 0) uniform UniformBufferObject_Scene
{
	float mGlossinessFactor;
	int mDiffuseTextureIndex;
	int mNormalTextureIndex;
	vec4 mDiffuseFactor;
	vec4 mAmbientColor;
}uboMaterial;

layout (set = 1, binding = 1) uniform sampler2D samplerColorMap;
layout (set = 1, binding = 2) uniform sampler2D samplerNormalMap;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inLightDir;
layout (location = 4) in vec3 inViewDir;


layout (location = 0) out vec4 outFragColor;

void main() 
{
	

	vec3 N = normalize(inNormal);
	if (uboMaterial.mNormalTextureIndex > 0)
	{
		vec4 normal = texture(samplerNormalMap, inUV);
		N = normal.xyz;
	}
	vec3 L = normalize(-inLightDir);
	vec3 sunLightColor = vec3(1.0f, 1.0f, 1.0f);
	float _dot = dot(N, L);
	vec3 lightColor = max(_dot, 0.5) * sunLightColor;
	
	//outFragColor = uboMaterial.mDiffuseFactor;
	vec4 color = texture(samplerColorMap, inUV);

	outFragColor = vec4(color.rgb * lightColor, 1.0f) + uboMaterial.mAmbientColor;
}