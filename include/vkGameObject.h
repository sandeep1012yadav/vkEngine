#pragma once
#include "vkCore.h"
#include "vkObject.h"

namespace vk
{
	struct vkVertex;
	class vkFrameObject;
	class vkGameObject : public vkObject
	{
	public:
		vkGameObject();
		vkGameObject(const std::string& name, vkFrameObject* pFrameObject);
		explicit vkGameObject(const std::string& name);
		~vkGameObject();

		void Preprocess();

		std::vector<vkVertex> m_vVertices;

		std::vector<uint32_t> m_vIndices;

		vkFrameObject* m_pFrameObject;

		uint32_t mNbMaterialDescriptorSets;

		glm::vec3 mPosition;
		glm::vec3 mRotation;
		glm::vec3 mScale;

		glm::mat4 mWorldTransform;

		const VkBuffer& GetVertexBuffer() const { return mVertexBuffer; }
		const VkBuffer& GetIndexBuffer() const { return mIndexBuffer; }
	private:
		VkBuffer mVertexBuffer;
		VkBuffer mIndexBuffer;

		VkDeviceMemory mVertexBufferMemory;
		VkDeviceMemory mIndexBufferMemory;

		uint32_t CalculateNbMaterialDescriptorSets(vkFrameObject* node);
		void UpdateTransform(vkFrameObject* currFrame, vkFrameObject* parentFrame);
	};
}
