#pragma once
#include "vkCore.h"

namespace vk
{
	class vkEngine;
	class vkScene;
	class vkPipelineManager;
	class vkDevice;
	class vkFrameObj;
	class vkRenderer
	{
	public:
		float RenderScene(const VkCommandBuffer& commandBuffer, float deltaTime);
		~vkRenderer();

	private:
		vkRenderer(const vkEngine* pEngine, const vkPipelineManager* pPipelineManager);

		void RenderFrame(const VkCommandBuffer& commandBuffer, vkFrameObject* pFrameObj);
		void UpdateScene(float deltaTime);

		friend class vkEngine;

		const vkEngine* m_pEngine;
		const vkPipelineManager* m_pPipelineManager;
		const vkDevice* m_pDevice;
		vkScene* m_pMainScene;

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};

	
}
