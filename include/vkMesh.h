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

		static const std::vector<VkVertexInputBindingDescription> GetBindingDesc()
		{
			const std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions = {
				vk::initializers::VertexInputBindingDescription(0, sizeof(vkVertex), VK_VERTEX_INPUT_RATE_VERTEX)
			};
			return vertexBindingDescriptions;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDesc()
		{
			const std::vector<VkVertexInputAttributeDescription> vertexAttributeDesc = {
				vk::initializers::VertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vkVertex,vPos)),
				vk::initializers::VertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertex, vTexCoord)),
				vk::initializers::VertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertex, vColor))
			};

			return vertexAttributeDesc;
		}
	};
}