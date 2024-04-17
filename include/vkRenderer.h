#pragma once
#include "vkCore.h"

namespace vk
{
	typedef struct _sceneUniformBufferObject
	{
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;

	}SceneUniformBufferObject;

	typedef struct _frameUniformBufferObject
	{
		glm::mat4 worldMatrix;

	}FrameUniformBufferObject;

	typedef struct _VulkanBuffer
	{
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;

	}VulkanBuffer;

	class vkEngine;
	class vkScene;
	class vkPipelineManager;
	class vkDevice;
	class vkFrameObj;
	class vkRenderer
	{
	public:
		float RenderScene(const VkCommandBuffer& commandBuffer, float fTimeElapsed);
		~vkRenderer();

	private:
		vkRenderer(const vkEngine* pEngine, const vkPipelineManager* pPipelineManager);
		bool CreateBuffers();
		bool CreateDescriptorPool();
		bool AllocateDescriptorSets();
		bool LinkDescriptorSetsToResources();

		void RenderFrame(const VkCommandBuffer& commandBuffer, vkFrameObject* pFrameObj, uint32_t& drawableFrameIndex);

		friend class vkEngine;

		const vkEngine* m_pEngine;
		const vkPipelineManager* m_pPipelineManager;
		const vkDevice* m_pDevice;
		vkScene* m_pMainScene;

		VkDescriptorPool m_DescriptorPool;


		SceneUniformBufferObject m_SceneUBO;
		FrameUniformBufferObject m_FrameUBO;

		uint32_t m_NbSceneBuffers;
		VkDescriptorSet* m_SceneDescriptorSets;
		VulkanBuffer* m_SceneBuffers;

		uint32_t m_NbFrameBuffers;
		VkDescriptorSet* m_FrameDescriptorSets;
		VulkanBuffer* m_FrameBuffers;

		VkPipelineLayout m_DefautPipelineLayout;
		VkPipeline m_DefaultPipeline;

	};

	
}
