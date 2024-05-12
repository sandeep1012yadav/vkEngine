#include "vkCore.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"
#include "vkMesh.h"
#include "vkEngine.h"
#include "vkDevice.h"
#include "vkResourcePool.h"
namespace vk
{
	vkGameObject::vkGameObject() : vkObject("noName", ObjectType::OT_GameObject)
	{
		m_pFrameObject = nullptr;
		mNbUniformBuffers = 0;

		mPosition = glm::vec3(0.0f);
		mRotation = glm::vec3(0.0f);
		mScale = glm::vec3(1.0f);
		mWorldTransform = glm::mat4(1.0f);

		mVertexBuffer = VK_NULL_HANDLE;
		mIndexBuffer = VK_NULL_HANDLE;
	}

	vkGameObject::vkGameObject(const std::string& name, vkFrameObject* pFrameObject) : vkObject(name, ObjectType::OT_GameObject)
	{
		m_pFrameObject = pFrameObject;
		mNbUniformBuffers = 0;

		mPosition = glm::vec3(0.0f);
		mRotation = glm::vec3(0.0f);
		mScale = glm::vec3(1.0f);
		mWorldTransform = glm::mat4(1.0f);

		uint32_t vIndices = 0;
		uint32_t iIndices = 0;
		IterateObjectForVertexArray(m_pFrameObject, vIndices);
		IterateObjectForIndexArray(m_pFrameObject, iIndices);

		mVertexBuffer = VK_NULL_HANDLE;
		mIndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

		bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateVertexBuffer(m_vVertices.size(), m_vVertices.data(), mVertexBuffer, vertexBufferMemory);
		if (bStatus == false) {
			vkLog->Log("Vertex buffer creation failed for the object - ", mName);
		}

		bStatus = vkEngine::GetInstance()->GetDevice()->CreateIndexBuffer(m_vIndices.size(), m_vIndices.data(), mIndexBuffer, indexBufferMemory);
		if (bStatus == false) {
			vkLog->Log("Index buffer creation failed for the object - ", mName);
		}

		vkLog->Log("GameObject : ", name, " created...");
	}

	vkGameObject::vkGameObject(const std::string& name) : vkObject(name, ObjectType::OT_GameObject)
	{
		m_pFrameObject = nullptr;
		mNbUniformBuffers = 0;

		mPosition = glm::vec3(0.0f);
		mRotation = glm::vec3(0.0f);
		mScale = glm::vec3(1.0f);
		mWorldTransform = glm::mat4(1.0f);

		mVertexBuffer = VK_NULL_HANDLE;
		mIndexBuffer = VK_NULL_HANDLE;
	}

	vkGameObject::~vkGameObject()
	{

	}

	void vkGameObject::Preprocess()
	{
		CalculateNbUniformBuffers(m_pFrameObject, mNbUniformBuffers);

		if (mVertexBuffer == VK_NULL_HANDLE)
		{
			bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateVertexBuffer(m_vVertices.size(), m_vVertices.data(), mVertexBuffer, mVertexBufferMemory);
			if (bStatus == false) {
				vkLog->Log("Vertex buffer creation failed for the object - ", mName);
			}
		}
		if (mIndexBuffer == VK_NULL_HANDLE)
		{
			bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateIndexBuffer(m_vIndices.size(), m_vIndices.data(), mIndexBuffer, mIndexBufferMemory);
			if (bStatus == false) {
				vkLog->Log("Index buffer creation failed for the object - ", mName);
			}
		}
	}

	uint32_t vkGameObject::CalculateNbVertices(vkFrameObject* node)
	{
		if (node == NULL)
			return 0;

		uint32_t nbVertices = 0;
		if (node->m_pMesh != nullptr)
		{
			nbVertices += node->m_pMesh->mNbVertices;
		}

		if (node->mNbChildren > 0)
		{
			for (uint32_t i = 0; i < node->mNbChildren; i++)
			{
				nbVertices += CalculateNbVertices(node->m_pChildren[i]);
			}
		}

		return nbVertices;
	}

	uint32_t vkGameObject::CalculateNbIndices(vkFrameObject* node)
	{
		if (node == NULL)
			return 0;

		uint32_t nbIndices = 0;
		if (node->m_pMesh != nullptr)
		{
			nbIndices += node->m_pMesh->mNbIndices;
		}

		if (node->mNbChildren > 0)
		{
			for (uint32_t i = 0; i < node->mNbChildren; i++)
			{
				nbIndices += CalculateNbIndices(node->m_pChildren[i]);
			}
		}

		return nbIndices;
	}

	void vkGameObject::CalculateNbUniformBuffers(vkFrameObject* node, uint32_t& nbUniformBuffers)
	{
		if (node == NULL)
			return;

		if (node->m_pMesh != nullptr)
		{
			nbUniformBuffers++;
		}

		for (uint32_t i = 0; i < node->mNbChildren; i++)
		{
			CalculateNbUniformBuffers(node->m_pChildren[i], nbUniformBuffers);
		}
	}

	void vkGameObject::IterateObjectForVertexArray(vkFrameObject* node, uint32_t& index)
	{
		/*if (node == NULL)
			return;

		if (node->m_pMesh != nullptr)
		{
			vkMesh* pMesh = node->m_pMesh;
			for (uint32_t i = 0; i < pMesh->m_vPrimitives.size(); i++)
			{
				vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(node->m_pMeshes[i]);

				pMesh->mStartVertexLocation = index;

				for (uint32_t j = 0; j < pMesh->mNbVertices; j++)
				{
					m_vVertices.push_back(pMesh->m_pVertices[j]);
					index++;
				}
			}
		}

		if (node->mNbChildren > 0)
		{
			for (uint32_t i = 0; i < node->mNbChildren; i++)
			{
				IterateObjectForVertexArray(node->m_pChildren[i], index);
			}
		}*/
	}

	void vkGameObject::IterateObjectForIndexArray(vkFrameObject* node, uint32_t& index)
	{
		//if (node == NULL)
		//	return;

		//if (node->mNbMeshes > 0)
		//{
		//	for (uint32_t i = 0; i < node->mNbMeshes; i++)
		//	{
		//		vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(node->m_pMeshes[i]);

		//		pMesh->mStartIndexLocation = index;

		//		for (uint32_t j = 0; j < pMesh->mNbIndices; j++)
		//		{
		//			m_vIndices.push_back(pMesh->m_pIndices[j]);
		//			index++;
		//		}
		//		//pMesh->mIndicesCount = index - pMesh->mStartIndexLocation;
		//	}
		//}

		//if (node->mNbChildren > 0)
		//{
		//	for (uint32_t i = 0; i < node->mNbChildren; i++)
		//	{
		//		IterateObjectForIndexArray(node->m_pChildren[i], index);
		//	}
		//}
	}

	void vkGameObject::UpdateTransform(vkFrameObject* currFrame, vkFrameObject* parentFrame, uint32_t& drawableFrameIndex)
	{

	}
}