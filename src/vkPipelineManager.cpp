#include "vkCore.h"
#include "vkPipeline.h"
#include "vkPipelineManager.h"
#include "vkEngine.h"
#include "vkDevice.h"
#include "vkLogger.h"

namespace vk
{
	vkPipelineManager::vkPipelineManager(vkEngine* pEngine)
	{
		m_pvkEngine = pEngine;
		m_LogicalDevice = pEngine->GetDevice()->GetLogicalDevice();
	}

	vkPipelineManager::~vkPipelineManager()
	{
	}

	bool vkPipelineManager::CreateDescriptorSetLayouts()
	{
		VkDescriptorSetLayout descriptorSetLayout;

		// creating descriptor set layout for global shader ///////////////////////////////////////////////////
		std::array<VkDescriptorSetLayoutBinding, 1> globalDescriptorSetLayoutBindings{};
		globalDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		globalDescriptorSetLayoutBindings[0].binding = 0;
		globalDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		globalDescriptorSetLayoutBindings[0].descriptorCount = 1;
		VkDescriptorSetLayoutCreateInfo globalDescriptorLayoutCI{};
		globalDescriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		globalDescriptorLayoutCI.bindingCount = static_cast<uint32_t>(globalDescriptorSetLayoutBindings.size());
		globalDescriptorLayoutCI.pBindings = globalDescriptorSetLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_LogicalDevice, &globalDescriptorLayoutCI, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			vkLog->Log("Global descriptor set layput creation failed.");
		}
		mDescriptorSetLayoutMap[DS_GLOBAL] = descriptorSetLayout;
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		// creating descriptor set layout for default shader //////////////////////////////////////////////////
		std::array<VkDescriptorSetLayoutBinding, 1> defaultDescriptorSetLayoutBindings{};
		defaultDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		defaultDescriptorSetLayoutBindings[0].binding = 0;
		defaultDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		defaultDescriptorSetLayoutBindings[0].descriptorCount = 1;
		VkDescriptorSetLayoutCreateInfo defaultDescriptorLayoutCI{};
		defaultDescriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		defaultDescriptorLayoutCI.bindingCount = static_cast<uint32_t>(defaultDescriptorSetLayoutBindings.size());
		defaultDescriptorLayoutCI.pBindings = defaultDescriptorSetLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_LogicalDevice, &defaultDescriptorLayoutCI, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			vkLog->Log("Default descriptor set layput creation failed.");
		}
		mDescriptorSetLayoutMap[DS_DEFAULT] = descriptorSetLayout;
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		return true;
	}

	VkDescriptorSetLayout vkPipelineManager::GetDescriptorSetLayout(eDescriptorSets set)
	{
		if (mDescriptorSetLayoutMap.find(set) != mDescriptorSetLayoutMap.end())
		{
			return mDescriptorSetLayoutMap[set];
		}
		return nullptr;
	}

	bool vkPipelineManager::CreatePipelineLayouts()
	{
		VkPipelineLayout pipelineLayout;
		
		// pipeline layout for default shader ////////////////////////////////////////////////////
		std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts;
		descriptorSetLayouts[0] = mDescriptorSetLayoutMap[DS_GLOBAL];
		descriptorSetLayouts[1] = mDescriptorSetLayoutMap[DS_DEFAULT];
		VkPipelineLayoutCreateInfo pipelineLayoutCI{};
		pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// The pipeline layout is based on the descriptor set layouts we created above
		pipelineLayoutCI.setLayoutCount = 2;
		pipelineLayoutCI.pSetLayouts = descriptorSetLayouts.data();
		if (vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			vkLog->Log("Default shader pipeline layout creation failed.");
		}
		mPipelineLayoutMap[PL_DEFAULT] = pipelineLayout;
		//////////////////////////////////////////////////////////////////////////////////////////
	
		
	}

	VkPipelineLayout vkPipelineManager::GetPipelineLayout(ePipelines pipeline)
	{
		if (mPipelineLayoutMap.find(pipeline) != mPipelineLayoutMap.end())
		{
			return mPipelineLayoutMap[pipeline];
		}
		return nullptr;
	}

	bool vkPipelineManager::CreateRenderPasses()
	{

	}

	VkRenderPass vkPipelineManager::GetRenderPass(ePipelines pipeline)
	{

	}
}