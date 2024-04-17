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

		uint32_t mNbVertices;
		vkVertex* m_pVertices;

		uint32_t mNbIndices;
		uint32_t* m_pIndices;

		vkFrameObject* m_pFrameObject;

		uint32_t mNbUniformBuffers;

		glm::vec3 mPosition;
		glm::vec3 mRotation;
		glm::vec3 mScale;

		glm::mat4 mWorldTransform;

		const VkBuffer& GetVertexBuffer() const { return mVertexBuffer; }
		const VkBuffer& GetIndexBuffer() const { return mIndexBuffer; }
	private:
		VkBuffer mVertexBuffer;
		VkBuffer mIndexBuffer;

		void CalculateNbUniformBuffers(vkFrameObject* node, uint32_t& nbUniformBuffers);
		uint32_t CalculateNbVertices(vkFrameObject* node);
		uint32_t CalculateNbIndices(vkFrameObject* node);
		void IterateObjectForVertexArray(vkFrameObject* node, uint32_t& index);
		void IterateObjectForIndexArray(vkFrameObject* node, uint32_t& index);
		void UpdateTransform(vkFrameObject* currFrame, vkFrameObject* parentFrame, uint32_t& drawableFrameIndex);
	};
}
