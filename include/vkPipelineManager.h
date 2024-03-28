#pragma once
#include "vkCore.h"
#include "vkPipeline.h"

namespace vk
{
	enum ePipeline
	{
		PL_DEFAULT
	};

	class vkEngine;
	class vkDevice;
	class vkPipelineManager
	{
	public:
		vkPipelineManager(vkEngine* pEngine);
		~vkPipelineManager();

		bool CreateDescriptorSetLayouts();
		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts(ePipeline pipeline);
		bool CreatePipelineLayouts();
		VkPipelineLayout GetPipelineLayout(ePipeline pipeline);
		bool CreateRenderPasses();
		VkRenderPass GetRenderPass(ePipeline pipeline);

		bool CreatePipelines();
	private:
		vkEngine* m_pvkEngine;
		VkDevice m_LogicalDevice;
		std::map<ePipeline, std::vector<VkDescriptorSetLayout>> mDescriptorSetLayoutsMap;
		std::map<ePipeline, VkPipelineLayout> mPipelineLayoutMap;
	};
}