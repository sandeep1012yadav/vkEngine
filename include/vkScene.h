#pragma once
#include "vkCore.h"
#include "vkObject.h"
namespace vk
{
	struct BufferDescriptorSetResources
	{
		VkDescriptorBufferInfo  bufferInfo;
		VkDeviceMemory		    bufferMemory;
	};

	struct SceneUniformBufferObject
	{
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;

	};
	class vkEngine;
	class vkFrameObject;
	class vkGameObject;
	class vkCamera;
	class vkDevice;
	class vkPipelineManager;
	class vkScene : public vkObject
	{
	public:
		~vkScene();

		uint32_t AddGameObject(vkGameObject* pGameObject);  // return gameobject id
		uint32_t AddCamera(vkCamera* pCamera); // return camera id;
		void SetMainCamera(uint32_t cameraId);

		vkCamera* GetCamera(uint32_t cameraId) const;
		vkCamera* GetMainCamera() const;
		vkGameObject* GetGameObject(uint32_t objId) const;

		const std::vector<vkGameObject*>& GetSceneGameObjects() const;

		uint32_t GetNbSceneDescriptorSets() const;
		uint32_t GetNbMaterialDescriptorSets() const;
		const VkDescriptorSet& GetSceneDescriptorSet() const { return m_SceneDescriptorSet; }
		void Preprocess();
		void Process(float deltaTime);

	private:
		vkScene(const vkEngine* pEngine);
		vkScene(const std::string& name, const vkEngine* pEngine);

		bool CreateDescriptorPool();
		void PreprocessFrameObject(vkFrameObject* pFrameObject, const VkDescriptorSetLayout& descriptorSetLayout);
		void PreprocessGameObject(vkGameObject* pGameObject, const VkDescriptorSetLayout& descriptorSetLayout);
		void ProcessFrameObject(vkFrameObject* pFrameObject);
		void ProcessGameObject(vkGameObject* pGameObject);
		
		const vkEngine* m_pEngine;
		const vkDevice* m_pDevice;
		const vkPipelineManager* m_pPipelineManager;
		std::vector<vkGameObject*> m_vSceneGameObjects;
		std::vector<vkCamera*> m_vSceneCameras;

		vkCamera* m_pMainCamera;
		uint32_t m_MainCameraId;

		VkDescriptorPool m_DescriptorPool;

		SceneUniformBufferObject m_SceneUBO;
		VkDescriptorSet m_SceneDescriptorSet;
		BufferDescriptorSetResources m_SceneDescriptorSetResources;

		uint32_t m_NbMaterialDescriptorSets;

		friend class vkEngine;
	};

	
}