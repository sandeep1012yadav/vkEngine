#pragma once
#include "vkCore.h"
#include "glm/glm.hpp"

namespace vk
{
	struct vkVertex
	{
		glm::vec4 vPos;
		glm::vec2 vTexCoord;
		glm::vec3 vColor;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription vertexBindingDesc{};
			vertexBindingDesc.binding = 0;
			vertexBindingDesc.stride = sizeof(vkVertex);
			vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return vertexBindingDesc;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDesciption()
		{
			std::array<VkVertexInputAttributeDescription, 3> vertexAttributeDesc{};
			vertexAttributeDesc[0].binding = 0;
			vertexAttributeDesc[0].location = 0;
			vertexAttributeDesc[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			vertexAttributeDesc[0].offset = offsetof(vkVertex, vPos);

			vertexAttributeDesc[1].binding = 0;
			vertexAttributeDesc[1].location = 1;
			vertexAttributeDesc[1].format = VK_FORMAT_R32G32_SFLOAT;
			vertexAttributeDesc[1].offset = offsetof(vkVertex, vTexCoord);

			vertexAttributeDesc[2].binding = 0;
			vertexAttributeDesc[2].location = 2;
			vertexAttributeDesc[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			vertexAttributeDesc[2].offset = offsetof(vkVertex, vColor);

			return vertexAttributeDesc;
		}
	};
}