#version 450
layout (set = 0, binding = 0) uniform UBOScene
{
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec3 lightDir;
	vec3 cameraPos;
}uboScene;

layout (set = 1, binding = 0) uniform UBOFrame
{
	mat4 modelMarrix;
} uboFrame;


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
	gl_Position = uboScene.projectionMatrix * uboScene.viewMatrix * uboFrame.modelMarrix * vec4(inPos.xyz, 1.0);
	outNormal = inNormal;
	outColor = inColor;
	outUV = inUV;
	outLightDir = uboScene.lightDir;
	outCameraPos = uboScene.cameraPos;
}
