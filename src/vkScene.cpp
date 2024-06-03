#include "vkEngine.h"
#include "vkCamera.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"
#include "vkDevice.h"
#include "vkMesh.h"
#include "vkMaterial.h"
#include "vkResourcePool.h"
#include "vkTexture.h"
#include "vkPipelineManager.h"
#include "vkScene.h"
namespace vk
{
	vkScene::vkScene(const vkEngine* pEngine) : vkObject("noName", ObjectType::OT_Scene), m_pEngine(pEngine)
	{
		m_pDevice = m_pEngine->GetDevice();
		m_pPipelineManager = m_pEngine->GetPipelineManager();
		m_pMainCamera = new vkCamera("SceneMainCamera");
		m_MainCameraId = 0;
		m_vSceneCameras.push_back(m_pMainCamera);

		m_NbMaterialDescriptorSets = 0;
	}

	vkScene::vkScene(const std::string& name, const vkEngine* pEngine) : vkObject(name, ObjectType::OT_Scene), m_pEngine(pEngine)
	{
		m_pDevice = m_pEngine->GetDevice();
		m_pPipelineManager = m_pEngine->GetPipelineManager();
		m_pMainCamera = new vkCamera("SceneMainCamera");
		m_MainCameraId = 0;
		m_vSceneCameras.push_back(m_pMainCamera);

		m_NbMaterialDescriptorSets = 0;
	}

	vkScene::~vkScene()
	{
		vkDestroyDescriptorPool(m_pDevice->GetLogicalDevice(), m_DescriptorPool, nullptr);
	}

	uint32_t vkScene::AddGameObject(vkGameObject* pGameObject)
	{
		uint32_t objIndex = static_cast<uint32_t>(m_vSceneGameObjects.size());
		m_vSceneGameObjects.push_back(pGameObject);
		return objIndex;
	}

	uint32_t vkScene::AddCamera(vkCamera* pCamera)
	{
		uint32_t camIndex = static_cast<uint32_t>(m_vSceneCameras.size());
		m_vSceneCameras.push_back(pCamera);
		return camIndex;
	}

	void vkScene::SetMainCamera(uint32_t cameraId)
	{
		m_MainCameraId = cameraId;
		m_pMainCamera = m_vSceneCameras[m_MainCameraId];
	}

	vkCamera* vkScene::GetCamera(uint32_t cameraId) const
	{
		return m_vSceneCameras[cameraId];
	}

	vkCamera* vkScene::GetMainCamera() const
	{
		return m_vSceneCameras[m_MainCameraId];
	}

	vkGameObject* vkScene::GetGameObject(uint32_t objId) const
	{
		return m_vSceneGameObjects[objId];
	}

	const std::vector<vkGameObject*>& vkScene::GetSceneGameObjects() const
	{
		return m_vSceneGameObjects;
	}

	uint32_t vkScene::GetNbSceneDescriptorSets() const
	{
		return 1;
	}

	uint32_t vkScene::GetNbMaterialDescriptorSets() const
	{
		return m_NbMaterialDescriptorSets;
	}

	bool vkScene::CreateDescriptorPool()
	{
		bool bStatus = true;

		uint32_t nbUniformBuffers = 1 + m_NbMaterialDescriptorSets;
		uint32_t nbCombinedImageSamplers = m_NbMaterialDescriptorSets;

		std::vector<VkDescriptorPoolSize> poolSizes = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nbUniformBuffers },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nbCombinedImageSamplers }
		};

		uint32_t maxSets = nbUniformBuffers;

		VkDescriptorPoolCreateInfo poolInfo = vk::initializers::DescriptorPoolCreateInfo(poolSizes, maxSets);

		VkResult result = vkCreateDescriptorPool(m_pDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool);
		if (result != VK_SUCCESS) {
			bStatus = false;
		}

		return bStatus;
	}

	void vkScene::PreprocessFrameObject(vkFrameObject* pFrameObject, const VkDescriptorSetLayout& descriptorSetLayout)
	{
		if (pFrameObject)
		{
			if (pFrameObject->m_pMesh != nullptr)
			{
				vkMesh* pMesh = pFrameObject->m_pMesh;
				for (uint32_t i = 0; i < pMesh->GetNbPrimitive(); i++)
				{
					uint32_t matIndex = pMesh->m_vPrimitives[i].mMaterialIndex;
					vkMaterial* pMaterial = vkResourcePool::GetInstance()->GetMaterial(matIndex);

					////create material descriptor set resources ///////////////////////////////////////////////
					{
						BufferImageDescriptorSetResources& descriptorSetResources = pMaterial->mMaterialDescriptorSetResources;
						VkDeviceSize bufferSize = static_cast<VkDeviceSize>(sizeof(MaterialUniformBufferObject));
						vkEngine::GetInstance()->GetDevice()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, descriptorSetResources.bufferInfo.buffer, descriptorSetResources.bufferMemory);

						descriptorSetResources.bufferInfo.offset = 0;
						descriptorSetResources.bufferInfo.range = sizeof(MaterialUniformBufferObject);
						if (pMaterial->mDiffuseTextureIndex >= 0)
						{
							vkTexture* pTexture = vkResourcePool::GetInstance()->GetTexture(pMaterial->mDiffuseTextureIndex);
							descriptorSetResources.imageInfo = pTexture->GetDescriptorImageInfo();
							descriptorSetResources.imageMemory = pTexture->GetDeviceMemory();
						}
					}
					//// updating material UBO ////////////////////////////////////
					{
						pMaterial->UpdateMaterialUniformBufferObject();
						vk::tools::UpdateUniformBuffer(m_pDevice->GetLogicalDevice(), pMaterial->mMaterialDescriptorSetResources.bufferMemory,
							&pMaterial->mMaterialUBO, sizeof(MaterialUniformBufferObject));
					}
					////allocate descriptor set from pool //////////////////////////////////
					{
						std::vector<VkDescriptorSetLayout> materialDescriptorSetLayouts(1, descriptorSetLayout);
						VkDescriptorSetAllocateInfo allocInfo = vk::initializers::DescriptorSetAllocateInfo(m_DescriptorPool, materialDescriptorSetLayouts);
						VkResult result = vkAllocateDescriptorSets(m_pDevice->GetLogicalDevice(), &allocInfo, &pMaterial->m_MaterialDescriptorSet);
						if (result != VK_SUCCESS) {
							vkLog->Log("vkScene : allocation of descriptor set failed...");
						}
					}
					//// linking descriptor set to resources ///////////////////////////////////
					{
						std::vector<VkWriteDescriptorSet> descriptorWrites;
						VkWriteDescriptorSet bufferDescriptorWrite = {};
						bufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						bufferDescriptorWrite.dstSet = pMaterial->m_MaterialDescriptorSet;
						bufferDescriptorWrite.dstBinding = 0;
						bufferDescriptorWrite.dstArrayElement = 0;
						bufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						bufferDescriptorWrite.descriptorCount = 1;
						bufferDescriptorWrite.pBufferInfo = &pMaterial->mMaterialDescriptorSetResources.bufferInfo;
						descriptorWrites.push_back(bufferDescriptorWrite);

						if (pMaterial->mDiffuseTextureIndex >= 0)
						{
							VkWriteDescriptorSet imageDescriptorWrite = {};
							imageDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							imageDescriptorWrite.dstSet = pMaterial->m_MaterialDescriptorSet;
							imageDescriptorWrite.dstBinding = 1; // Binding number for the combined image sampler
							imageDescriptorWrite.dstArrayElement = 0;
							imageDescriptorWrite.descriptorCount = 1;
							imageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							imageDescriptorWrite.pImageInfo = &pMaterial->mMaterialDescriptorSetResources.imageInfo;
							descriptorWrites.push_back(imageDescriptorWrite);
						}
						vkUpdateDescriptorSets(m_pDevice->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
					}
					////////////////////////////////////////////////////////////////////////////////////////////
				}
			}

			for (uint32_t i = 0; i < pFrameObject->mNbChildren; i++)
			{
				PreprocessFrameObject(pFrameObject->m_pChildren[i], descriptorSetLayout);
			}
		}
	}

	void vkScene::PreprocessGameObject(vkGameObject* pGameObject, const VkDescriptorSetLayout& descriptorSetLayout)
	{
		PreprocessFrameObject(pGameObject->m_pFrameObject, descriptorSetLayout);
	}

	void vkScene::Preprocess()
	{
		m_NbMaterialDescriptorSets = 0;
		for (auto it = m_vSceneGameObjects.begin(); it != m_vSceneGameObjects.end(); it++)
		{
			vkGameObject* obj = *it;
			obj->Preprocess();
			m_NbMaterialDescriptorSets += obj->mNbMaterialDescriptorSets;
		}

		CreateDescriptorPool();	

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = m_pPipelineManager->GetDescriptorSetLayouts(ePipeline::PL_STANDARD);

		VkDescriptorSetLayout sceneDescriptorSetLayout = descriptorSetLayouts[0];
		VkDescriptorSetLayout materialDescriptorSetLayout = descriptorSetLayouts[1];

		bool bStatus = true;
		////// create scene buffers (scene descriptor resource) ////////////////
		{
			VkDeviceSize bufferSize = static_cast<VkDeviceSize>(sizeof(SceneUniformBufferObject));
			bStatus &= m_pDevice->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_SceneDescriptorSetResources.bufferInfo.buffer, m_SceneDescriptorSetResources.bufferMemory);

			m_SceneDescriptorSetResources.bufferInfo.offset = 0;
			m_SceneDescriptorSetResources.bufferInfo.range = sizeof(SceneUniformBufferObject);
		}
		//// allocate descriptor set from pool //////////////////////////////////
		{
			std::vector<VkDescriptorSetLayout> sceneDescriptorSetLayouts(1, sceneDescriptorSetLayout);
			VkDescriptorSetAllocateInfo allocInfo = vk::initializers::DescriptorSetAllocateInfo(m_DescriptorPool, sceneDescriptorSetLayouts);
			VkResult result = vkAllocateDescriptorSets(m_pDevice->GetLogicalDevice(), &allocInfo, &m_SceneDescriptorSet);
			if (result != VK_SUCCESS) {
				bStatus = false;
			}
		}
		//// linking descriptor set to resources ///////////////////////////////////
		{
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_SceneDescriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &m_SceneDescriptorSetResources.bufferInfo;

			vkUpdateDescriptorSets(m_pDevice->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}
		////////////////////////////////////////////////////////////////////////////////

		for (auto it = m_vSceneGameObjects.begin(); it != m_vSceneGameObjects.end(); it++)
		{
			vkGameObject* obj = *it;
			PreprocessGameObject(obj, materialDescriptorSetLayout);
		}
	}

	void vkScene::ProcessFrameObject(vkFrameObject* pFrameObject)
	{
		if (pFrameObject)
		{
			if (pFrameObject->m_pParent)
			{
				pFrameObject->mWorldTransformation = pFrameObject->m_pParent->mWorldTransformation * pFrameObject->mLocalTransformation;
			}
			else
			{
				pFrameObject->mWorldTransformation = pFrameObject->mLocalTransformation;
			}
			
			for (uint32_t i = 0; i < pFrameObject->mNbChildren; i++)
			{
				ProcessFrameObject(pFrameObject->m_pChildren[i]);
			}
		}
	}

	void vkScene::ProcessGameObject(vkGameObject* pGameObject)
	{
		ProcessFrameObject(pGameObject->m_pFrameObject);
	}

	void vkScene::Process(float deltaTime)
	{
		m_pMainCamera->Update(deltaTime);
		//// update scene descriptor set resources /////
		{
			m_SceneUBO.projMatrix = m_pMainCamera->GetProjectionMatrix();
			m_SceneUBO.viewMatrix = m_pMainCamera->GetViewMatrix();
			vk::tools::UpdateUniformBuffer(m_pDevice->GetLogicalDevice(), m_SceneDescriptorSetResources.bufferMemory,
				&m_SceneUBO, sizeof(SceneUniformBufferObject));
		}
		//// update game object descriptor set resources or push constants
		{
			for (auto it = m_vSceneGameObjects.begin(); it != m_vSceneGameObjects.end(); it++)
			{
				vkGameObject* obj = *it;
				ProcessGameObject(obj);
			}
		}
	}
}