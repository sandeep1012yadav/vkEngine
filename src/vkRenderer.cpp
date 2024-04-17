#include "vkCore.h"
#include "vkEngine.h"
#include "vkScene.h"
#include "vkDevice.h"
#include "vkPipelineManager.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"
#include "vkMesh.h"
#include "vkResourcePool.h"
#include "vkRenderer.h"
namespace vk
{
	vkRenderer::vkRenderer(const vkEngine* pEngine, const vkPipelineManager* pPipelineManager) : m_pEngine(pEngine), m_pPipelineManager(pPipelineManager)
	{
		m_pDevice = m_pEngine->GetDevice();
		m_pMainScene = m_pEngine->GetMainScene();

		m_pMainScene->Preprocess();

		m_DefautPipelineLayout = m_pPipelineManager->GetPipelineLayout(ePipeline::PL_DEFAULT);
		m_DefaultPipeline = m_pPipelineManager->GetPipeline(ePipeline::PL_DEFAULT);

		bool bStatus = CreateBuffers();
		if (bStatus == false) {
			vkLog->Log("vkRenderer : Creation of Buffers failed...");
		}

		bStatus = CreateDescriptorPool();
		if (bStatus == false) {
			vkLog->Log("vkRenderer : Creation of Descriptor Pool failed...");
		}

		bStatus = AllocateDescriptorSets();
		if (bStatus == false) {
			vkLog->Log("vkRenderer : Allocation of Descriptor Sets failed...");
		}

		bStatus = LinkDescriptorSetsToResources();
		if (bStatus == false) {
			vkLog->Log("vkRenderer : Linking Descriptor Sets to actual resources failed...");
		}
	}

	vkRenderer::~vkRenderer()
	{
		vkDestroyDescriptorPool(m_pDevice->GetLogicalDevice(), m_DescriptorPool, nullptr);
		delete m_SceneBuffers;
		delete m_FrameBuffers;

		delete m_SceneDescriptorSets;
		delete m_FrameDescriptorSets;
	}

	bool vkRenderer::CreateBuffers()
	{
		bool bStatus = true;

		m_NbSceneBuffers = m_pMainScene->GetNbSceneUniformBuffers();
		m_NbFrameBuffers = m_pMainScene->GetNbFrameUniformBuffers();

		m_SceneBuffers = new VulkanBuffer[m_NbSceneBuffers];
		m_FrameBuffers = new VulkanBuffer[m_NbFrameBuffers];

		VkDeviceSize bufferSize = static_cast<VkDeviceSize>(sizeof(SceneUniformBufferObject));
		for (uint32_t sceneBufferIndex = 0; sceneBufferIndex < m_NbSceneBuffers; sceneBufferIndex++)
		{
			bStatus &= m_pDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_SceneBuffers[sceneBufferIndex].buffer, m_SceneBuffers[sceneBufferIndex].bufferMemory);
		}

		bufferSize = static_cast<VkDeviceSize>(sizeof(FrameUniformBufferObject));
		for (uint32_t frameBufferIndex = 0; frameBufferIndex < m_NbFrameBuffers; frameBufferIndex++)
		{
			bStatus &= m_pDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_FrameBuffers[frameBufferIndex].buffer, m_FrameBuffers[frameBufferIndex].bufferMemory);
		}

		return bStatus;
	}

	bool vkRenderer::CreateDescriptorPool()
	{
		bool bStatus = true;
		uint32_t nbBuffers = m_NbSceneBuffers + m_NbFrameBuffers;
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nbBuffers }
		};

		uint32_t maxSets = nbBuffers;

		VkDescriptorPoolCreateInfo poolInfo = vk::initializers::DescriptorPoolCreateInfo(poolSizes, maxSets);

		VkResult result = vkCreateDescriptorPool(m_pDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool);
		if (result != VK_SUCCESS) {
			bStatus = false;
		}

		return false;
	}

	bool vkRenderer::AllocateDescriptorSets()
	{
		bool bStatus = true;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = m_pPipelineManager->GetDescriptorSetLayouts(ePipeline::PL_DEFAULT);

		m_SceneDescriptorSets = new VkDescriptorSet[m_NbSceneBuffers];
		std::vector<VkDescriptorSetLayout> sceneDescriptorSetLayouts(m_NbSceneBuffers, descriptorSetLayouts[0]);
		VkDescriptorSetAllocateInfo allocInfo = vk::initializers::DescriptorSetAllocateInfo(m_DescriptorPool, sceneDescriptorSetLayouts);
		VkResult result = vkAllocateDescriptorSets(m_pDevice->GetLogicalDevice(), &allocInfo, m_SceneDescriptorSets);
		if (result != VK_SUCCESS) {
			bStatus = false;
		}

		m_FrameDescriptorSets = new VkDescriptorSet[m_NbFrameBuffers];
		std::vector<VkDescriptorSetLayout> frameDescriptorSetLayouts(m_NbFrameBuffers, descriptorSetLayouts[1]);
		allocInfo = vk::initializers::DescriptorSetAllocateInfo(m_DescriptorPool, frameDescriptorSetLayouts);
		result = vkAllocateDescriptorSets(m_pDevice->GetLogicalDevice(), &allocInfo, m_FrameDescriptorSets);
		if (result != VK_SUCCESS) {
			bStatus = false;
		}

		return bStatus;
	}

	bool vkRenderer::LinkDescriptorSetsToResources()
	{
		bool bStatus = true;

		for (uint32_t sceneBufferIndex = 0; sceneBufferIndex < m_NbSceneBuffers; sceneBufferIndex++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_SceneBuffers[sceneBufferIndex].buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(SceneUniformBufferObject);

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_SceneDescriptorSets[sceneBufferIndex];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(m_pDevice->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		for (uint32_t frameBufferIndex = 0; frameBufferIndex < m_NbFrameBuffers; frameBufferIndex++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_FrameBuffers[frameBufferIndex].buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(FrameUniformBufferObject);

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_FrameDescriptorSets[frameBufferIndex];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(m_pDevice->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		return bStatus;
	}

	void vkRenderer::RenderFrame(const VkCommandBuffer& commandBuffer, vkFrameObject* pFrameObj, uint32_t& drawableFrameIndex)
	{
		if (pFrameObj == NULL)
			return;

		if (pFrameObj->mNbMeshes > 0)
		{
			VkDescriptorSet frameDescriptorSets[] = { m_FrameDescriptorSets[drawableFrameIndex] };
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_DefautPipelineLayout, 1, 1, frameDescriptorSets, 0, nullptr);
			for (uint32_t i = 0; i < pFrameObj->mNbMeshes; i++)
			{
				vkMesh* pMesh = vkResourcePool::GetInstance()->GetMesh(pFrameObj->m_pMeshes[i]);
				vkCmdDrawIndexed(commandBuffer, pMesh->mNbIndices, 1, pMesh->mStartIndexLocation, 0, 1);
			}
			drawableFrameIndex++;
		}
		for (uint32_t i = 0; i < pFrameObj->mNbChildren; i++)
		{
			RenderFrame(commandBuffer, pFrameObj->m_pChildren[i], drawableFrameIndex);
		}
	}

	float vkRenderer::RenderScene(const VkCommandBuffer& commandBuffer, float fTimeElapsed)
	{
		std::vector<vkGameObject*> sceneGameObjects = m_pMainScene->GetSceneGameObjects();
		uint32_t nbGameObjects = static_cast<uint32_t>(sceneGameObjects.size());

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_DefaultPipeline);
		VkDescriptorSet sceneDescriptorSets[] = { m_SceneDescriptorSets[0] };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_DefautPipelineLayout, 0, 1, sceneDescriptorSets, 0, nullptr);

		for (uint32_t i = 0; i < nbGameObjects; i++)
		{
			vkGameObject* pGameObject = sceneGameObjects[i];

			VkBuffer vertexBuffers[] = { pGameObject->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			
			VkBuffer indexBuffer = pGameObject->GetIndexBuffer();
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			uint32_t frameIndex = 0;
			RenderFrame(commandBuffer, pGameObject->m_pFrameObject, frameIndex);
		}

		return 0;
	}
}