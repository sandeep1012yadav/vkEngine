#include "vkCore.h"
#include "vkEngine.h"
#include "vkScene.h"
#include "vkDevice.h"
#include "vkPipelineManager.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"
#include "vkMesh.h"
#include "vkResourcePool.h"
#include "vkMaterial.h"
#include "vkRenderer.h"
#include "vkCamera.h"
namespace vk
{
	vkRenderer::vkRenderer(const vkEngine* pEngine, const vkPipelineManager* pPipelineManager) : m_pEngine(pEngine), m_pPipelineManager(pPipelineManager)
	{
		m_pDevice = m_pEngine->GetDevice();
		m_pMainScene = m_pEngine->GetMainScene();

		m_pMainScene->Preprocess();

		m_PipelineLayout = m_pPipelineManager->GetPipelineLayout(ePipeline::PL_STANDARD);
		m_Pipeline = m_pPipelineManager->GetPipeline(ePipeline::PL_STANDARD);
	}

	vkRenderer::~vkRenderer()
	{
	}

	void vkRenderer::UpdateScene(float deltaTime)
	{
		if (m_pMainScene)
		{
			m_pMainScene->Process(deltaTime);
		}
	}

	void vkRenderer::RenderFrame(const VkCommandBuffer& commandBuffer, vkFrameObject* pFrameObj)
	{
		if (pFrameObj == NULL)
			return;

		if (pFrameObj->m_pMesh != nullptr)
		{
			FramePushConstant pushConstant = { pFrameObj->mWorldTransformation };
			vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FramePushConstant), &pushConstant);

			vkMesh* pMesh = pFrameObj->m_pMesh;
			for (uint32_t i = 0; i < pMesh->GetNbPrimitive(); i++)
			{
				uint32_t matIndex = pMesh->m_vPrimitives[i].mMaterialIndex;
				vkMaterial* pMaterial = vkResourcePool::GetInstance()->GetMaterial(matIndex);
				VkDescriptorSet materialDescriptorSets[] = { pMaterial->GetMaterialDescriptorSet() };
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 1, 1, materialDescriptorSets, 0, nullptr);

				vkPrimitive primitive = pMesh->m_vPrimitives[i];
				vkCmdDrawIndexed(commandBuffer, primitive.mNbIndices, 1, primitive.mStartIndex, 0, 0);
			}
		}
		for (uint32_t i = 0; i < pFrameObj->mNbChildren; i++)
		{
			RenderFrame(commandBuffer, pFrameObj->m_pChildren[i]);
		}
	}

	float vkRenderer::RenderScene(const VkCommandBuffer& commandBuffer, float deltaTime)
	{
		UpdateScene(deltaTime);

		std::vector<vkGameObject*> sceneGameObjects = m_pMainScene->GetSceneGameObjects();
		uint32_t nbGameObjects = static_cast<uint32_t>(sceneGameObjects.size());

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		VkDescriptorSet sceneDescriptorSets[] = { m_pMainScene->GetSceneDescriptorSet() };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, sceneDescriptorSets, 0, nullptr);

		for (uint32_t i = 0; i < nbGameObjects; i++)
		{
			vkGameObject* pGameObject = sceneGameObjects[i];

			VkBuffer vertexBuffers[] = { pGameObject->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			
			VkBuffer indexBuffer = pGameObject->GetIndexBuffer();
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			RenderFrame(commandBuffer, pGameObject->m_pFrameObject);
		}

		return 0;
	}

	
}