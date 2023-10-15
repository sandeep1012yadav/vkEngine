#pragma once
#include "vkCore.h"
#include "vkPipeline.h"

namespace vk
{
	enum eDescriptorSets
	{
		DS_GLOBAL,
		DS_DEFAULT
	};
	enum ePipelines
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
		VkDescriptorSetLayout GetDescriptorSetLayout(eDescriptorSets set);
		bool CreatePipelineLayouts();
		VkPipelineLayout GetPipelineLayout(ePipelines pipeline);
		bool CreateRenderPasses();
		VkRenderPass GetRenderPass(ePipelines pipeline);
	private:
		vkEngine* m_pvkEngine;
		VkDevice m_LogicalDevice;
		std::map<eDescriptorSets, VkDescriptorSetLayout> mDescriptorSetLayoutMap;
		std::map<ePipelines, VkPipelineLayout> mPipelineLayoutMap;
	};
}