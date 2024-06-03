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

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inLightDir;
layout (location = 4) in vec3 inCameraPos;


layout (location = 0) out vec4 outFragColor;

void main() 
{
	//outFragColor = uboMaterial.mDiffuseFactor;
	outFragColor = texture(samplerColorMap, inUV) * uboMaterial.mDiffuseFactor;
}