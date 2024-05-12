#pragma once
#include "vkCore.h"
#include "vkObject.h"

namespace vk
{
	class vkGameObject;

	struct vkVertex
	{
		glm::vec4 vPos;
		glm::vec3 vNormal;
		glm::vec3 vColor;
		glm::vec2 vTexCoord;

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
				vk::initializers::VertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertex, vNormal)),
				vk::initializers::VertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertex, vColor)),
				vk::initializers::VertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertex, vTexCoord))
			};

			return vertexAttributeDesc;
		}
	};

	struct vkVertexTan
	{
		glm::vec4 vPos;
		glm::vec3 vNormal;
		glm::vec3 vTangent;
		glm::vec3 vColor;
		glm::vec2 vTexCoord;

		static const std::vector<VkVertexInputBindingDescription> GetBindingDesc()
		{
			const std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions = {
				vk::initializers::VertexInputBindingDescription(0, sizeof(vkVertexTan), VK_VERTEX_INPUT_RATE_VERTEX)
			};
			return vertexBindingDescriptions;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDesc()
		{
			const std::vector<VkVertexInputAttributeDescription> vertexAttributeDesc = {
				vk::initializers::VertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vkVertexTan,vPos)),
				vk::initializers::VertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertexTan, vNormal)),
				vk::initializers::VertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertexTan, vTangent)),
				vk::initializers::VertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertexTan, vColor)),
				vk::initializers::VertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertexTan, vTexCoord))
			};

			return vertexAttributeDesc;
		}
	};

	struct vkVertexFull
	{
		glm::vec4 vPos;
		glm::vec3 vNormal;
		glm::vec3 vTangent;
		glm::vec3 vColor;
		glm::vec2 vTexCoord0;
		glm::vec2 vTexCoord1;
		glm::vec2 vTexCoord2;
		glm::vec2 vTexCoord3;

		static const std::vector<VkVertexInputBindingDescription> GetBindingDesc()
		{
			const std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions = {
				vk::initializers::VertexInputBindingDescription(0, sizeof(vkVertexFull), VK_VERTEX_INPUT_RATE_VERTEX)
			};
			return vertexBindingDescriptions;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDesc()
		{
			const std::vector<VkVertexInputAttributeDescription> vertexAttributeDesc = {
				vk::initializers::VertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vkVertexFull,vPos)),
				vk::initializers::VertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertexFull, vNormal)),
				vk::initializers::VertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertexFull, vTangent)),
				vk::initializers::VertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vkVertexFull, vColor)),
				vk::initializers::VertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertexFull, vTexCoord0)),
				vk::initializers::VertexInputAttributeDescription(0, 5, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertexFull, vTexCoord1)),
				vk::initializers::VertexInputAttributeDescription(0, 6, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertexFull, vTexCoord2)),
				vk::initializers::VertexInputAttributeDescription(0, 7, VK_FORMAT_R32G32_SFLOAT, offsetof(vkVertexFull, vTexCoord3))
			};

			return vertexAttributeDesc;
		}
	};

	struct vkPrimitive
	{
		uint32_t mStartIndex;
		uint32_t mNbIndices;
		int32_t  mMaterialIndex;
	};

	class vkMesh : public vkObject
	{
	public:
		vkMesh();
		explicit vkMesh(const std::string name);
		~vkMesh();

		uint32_t GetNbPrimitive() const { return static_cast<uint32_t>(m_vPrimitives.size()); }
		void AddPrimitive(const vkPrimitive& primitive);

		vkGameObject* m_pParentGameObject;
		std::vector<vkPrimitive> m_vPrimitives;

		uint32_t mStartVertexLocation;
		uint32_t mNbVertices;
		uint32_t mStartIndexLocation;
		uint32_t mNbIndices;
	};

	
}