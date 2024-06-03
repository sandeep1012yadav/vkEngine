#include "vkCore.h"
#include "vkEngine.h"
#include "vkFrameObject.h"
#include "vkMesh.h"
#include "vkEngine.h"
#include "vkDevice.h"
#include "vkMaterial.h" 
#include "vkResourcePool.h"
#include "vkTexture.h"
#include "vkGameObject.h"
namespace vk
{
	vkGameObject::vkGameObject() : vkObject("noName", ObjectType::OT_GameObject)
	{
		m_pFrameObject = nullptr;
		//mNbUniformBuffers = 0;

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
		//mNbUniformBuffers = 0;

		mPosition = glm::vec3(0.0f);
		mRotation = glm::vec3(0.0f);
		mScale = glm::vec3(1.0f);
		mWorldTransform = glm::mat4(1.0f);

		uint32_t vIndices = 0;
		uint32_t iIndices = 0;

		mVertexBuffer = VK_NULL_HANDLE;
		mIndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

		bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateVertexBuffer(static_cast<uint32_t>(m_vVertices.size()), m_vVertices.data(), mVertexBuffer, vertexBufferMemory);
		if (bStatus == false) {
			vkLog->Log("Vertex buffer creation failed for the object - ", mName);
		}

		bStatus = vkEngine::GetInstance()->GetDevice()->CreateIndexBuffer(static_cast<uint32_t>(m_vIndices.size()), m_vIndices.data(), mIndexBuffer, indexBufferMemory);
		if (bStatus == false) {
			vkLog->Log("Index buffer creation failed for the object - ", mName);
		}

		vkLog->Log("GameObject : ", name, " created...");
	}

	vkGameObject::vkGameObject(const std::string& name) : vkObject(name, ObjectType::OT_GameObject)
	{
		m_pFrameObject = nullptr;
		//mNbUniformBuffers = 0;

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
		if (mVertexBuffer == VK_NULL_HANDLE)
		{
			bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateVertexBuffer(static_cast<uint32_t>(m_vVertices.size()), m_vVertices.data(), mVertexBuffer, mVertexBufferMemory);
			if (bStatus == false) {
				vkLog->Log("Vertex buffer creation failed for the object - ", mName);
			}
		}
		if (mIndexBuffer == VK_NULL_HANDLE)
		{
			bool bStatus = vkEngine::GetInstance()->GetDevice()->CreateIndexBuffer(static_cast<uint32_t>(m_vIndices.size()), m_vIndices.data(), mIndexBuffer, mIndexBufferMemory);
			if (bStatus == false) {
				vkLog->Log("Index buffer creation failed for the object - ", mName);
			}
		}

		if (mNbMaterialDescriptorSets == 0)
		{
			mNbMaterialDescriptorSets = CalculateNbMaterialDescriptorSets(m_pFrameObject);
		}
	}

	uint32_t vkGameObject::CalculateNbMaterialDescriptorSets(vkFrameObject* node)
	{
		if (node == NULL)
			return 0;

		uint32_t nbMaterialDS = 0;
		if (node->m_pMesh != nullptr)
		{
			nbMaterialDS += node->m_pMesh->GetNbPrimitive();
		}

		for (uint32_t i = 0; i < node->mNbChildren; i++)
		{
			nbMaterialDS += CalculateNbMaterialDescriptorSets(node->m_pChildren[i]);
		}
		return nbMaterialDS;
	}

	void vkGameObject::UpdateTransform(vkFrameObject* currFrame, vkFrameObject* parentFrame)
	{

	}
}