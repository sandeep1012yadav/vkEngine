#version 450
layout (set = 0, binding = 0) uniform UniformBufferObject_Scene
{
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec3 lightDir;
	vec3 cameraPos;
}uboScene;

layout (push_constant) uniform PushConstant_Frame
{
	mat4 modelMatrix;
} pcFrame;


layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;


layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outLightDir;
layout (location = 4) out vec3 outCameraPos;


void main() 
{
	gl_Position = uboScene.projectionMatrix * uboScene.viewMatrix * pcFrame.modelMatrix * vec4(inPos.xyz, 1.0);
	outNormal = inNormal;
	outColor = inColor;
	outUV = inUV;
	outLightDir = uboScene.lightDir;
	outCameraPos = uboScene.cameraPos;
}
