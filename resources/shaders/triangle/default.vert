#version 450
#include "../scene_global.h"

layout (set = 1, binding = 0) uniform transform 
{
	mat4 modelMarrix;
} transform_ubo;


layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;


layout (location = 0) out vec3 outColor;

out gl_PerVertex 
{
    vec4 gl_Position;
};


void main() 
{
	outColor = inColor;
	gl_Position = global_scene_ubo.projectionMatrix * global_scene_ubo.viewMatrix * transform_ubo.modelMarrix * vec4(inPos.xyz, 1.0);
}
