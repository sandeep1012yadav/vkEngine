#pragma once
#include "vkCore.h"
#include "vkPipeline.h"

namespace vk
{
	enum class ePipeline
	{
		PL_DEFAULT,
		PL_STANDARD
	};

	enum class eRenderPass
	{
		RP_Forward_Rendering_Geometry
	};

	class vkEngine;
	class vkDevice;
	class vkPipelineManager
	{
	public:
		vkPipelineManager(vkEngine* pEngine);
		~vkPipelineManager();

		bool CreateDescriptorSetLayouts();
		bool CreatePipelineLayouts();
		bool CreateRenderPasses();
		bool CreatePipelines();
		
		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts(ePipeline pipeline) const;
		VkPipelineLayout GetPipelineLayout(ePipeline pipeline) const;
		VkRenderPass GetRenderPass(eRenderPass renderPass) const;
		VkPipeline GetPipeline(ePipeline pipeline) const;

		
	private:
		vkEngine* m_pvkEngine;
		VkDevice m_vkLogicalDevice;
		std::map<eRenderPass, VkRenderPass> m_vkRenderPasses;
		std::map<ePipeline, std::vector<VkDescriptorSetLayout>> m_vkDescriptorSetLayoutsMap;
		std::map<ePipeline, VkPipelineLayout> m_vkPipelineLayoutMap;
		std::map<ePipeline, VkPipeline> m_vkPipelineMap;
	};
}