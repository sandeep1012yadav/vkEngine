#version 450
layout (set = 0, binding = 0) uniform scene_global
{
	mat4 viewMatrix;
	mat4 projectionMatrix;
}global_scene_ubo;

layout (set = 1, binding = 0) uniform transform 
{
	mat4 modelMarrix;
} transform_ubo;


layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;


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
