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
		mNbVertices = 0;
		m_pVertices = nullptr;

		mNbIndices = 0;
		m_pIndices = nullptr;

		m_pFrameObject = nullptr;

		mPosition = glm::vec3(0.0f);
		mRotation = glm::vec3(0.0f);
		mScale = glm::vec3(1.0f);
		mWorldTransform = glm::mat4(1.0f);

		mVertexBuffer = VK_NULL_HANDLE;
		mIndexBuffer = VK_NULL_HANDLE;
	}

	vkGameObject::vkGameObject(const std::string& name, vkFrameObject* pFrameObject) : vkObject(name, ObjectType::OT_GameObject)
	{
		mNbVertices = 0;
		m_pVertices = nullptr;

		mNbIndices = 0;
		m_pIndices = nullptr;

		m_pFrameObject = pFrameObject;
		mNbUniformBuffers = 0;

		mPosition = glm::vec3(0.0f);
		mRotation = glm::vec3(0.0f);
		mScale = glm::vec3(1.0f);
		mWorldTransform = glm::mat4(1.0f);

		mNbVertices = CalculateNbVertices(m_pFrameObject);
		mNbIndices = CalculateNbIndices(m_pFrameObject);

		m_pVertices = new vkVertex[mNbVertices];
		m_pIndices = new uint32_t[mNbIndices];

		uint32_t vIndices = 0;
		uint32_t iIndices = 0;
		IterateObjectForVertexArray(m_pFrameObject, vIndices);
		IterateObjectForIndexArray(m_pFrameObject, iIndices);

		mVertexBuffer = VK_NULL_HANDLE;
		mIndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

		bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateVertexBuffer(mNbVertices, m_pVertices, mVertexBuffer, vertexBufferMemory);
		if (bStatus == false) {
			vkLog->Log("Vertex buffer creation failed for the object - ", name);
		}

		bStatus = vkEngine::GetInstance()->GetDevice()->CreateIndexBuffer(mNbIndices, m_pIndices, mIndexBuffer, indexBufferMemory);
		if (bStatus == false) {
			vkLog->Log("Index buffer creation failed for the object - ", name);
		}

		vkLog->Log("GameObject : ", name, " created...");
	}

	vkGameObject::vkGameObject(const std::string& name) : vkObject(name, ObjectType::OT_GameObject)
	{
		mNbVertices = 0;
		m_pVertices = nullptr;

		mNbIndices = 0;
		m_pIndices = nullptr;

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
	}

	uint32_t vkGameObject::CalculateNbVertices(vkFrameObject* node)
	{
		if (node == NULL)
			return 0;

		uint32_t nbVertices = 0;
		if (node->mNbMeshes > 0)
		{
			for (uint32_t i = 0; i < node->mNbMeshes; i++)
			{
				vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(node->m_pMeshes[i]);
				nbVertices += pMesh->mNbVertices;
			}
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
		if (node->mNbMeshes > 0)
		{
			for (uint32_t i = 0; i < node->mNbMeshes; i++)
			{
				vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(node->m_pMeshes[i]);
				nbIndices += pMesh->mNbIndices;
			}
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

		if (node->mNbMeshes > 0)
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
		if (node == NULL)
			return;

		if (node->mNbMeshes > 0)
		{
			for (uint32_t i = 0; i < node->mNbMeshes; i++)
			{
				vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(node->m_pMeshes[i]);

				pMesh->mStartVertexLocation = index;

				for (uint32_t j = 0; j < pMesh->mNbVertices; j++)
				{
					m_pVertices[index] = pMesh->m_pVertices[j];
					/*if (pMesh->nbUVChannels > 0)
					{
						mVertexArray[index].texCoord = pMesh->mTextureCoords[0][j];
					}*/
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
		}
	}

	void vkGameObject::IterateObjectForIndexArray(vkFrameObject* node, uint32_t& index)
	{
		if (node == NULL)
			return;

		if (node->mNbMeshes > 0)
		{
			for (uint32_t i = 0; i < node->mNbMeshes; i++)
			{
				vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(node->m_pMeshes[i]);

				pMesh->mStartIndexLocation = index;

				for (uint32_t j = 0; j < pMesh->mNbIndices; j++)
				{
					m_pIndices[index] = pMesh->m_pIndices[j];
					index++;
				}
				//pMesh->mIndicesCount = index - pMesh->mStartIndexLocation;
			}
		}

		if (node->mNbChildren > 0)
		{
			for (uint32_t i = 0; i < node->mNbChildren; i++)
			{
				IterateObjectForIndexArray(node->m_pChildren[i], index);
			}
		}
	}

	void vkGameObject::UpdateTransform(vkFrameObject* currFrame, vkFrameObject* parentFrame, uint32_t& drawableFrameIndex)
	{

	}
}