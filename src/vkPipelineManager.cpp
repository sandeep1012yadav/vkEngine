#include "vkCore.h"
#include "vkPipeline.h"
#include "vkPipelineManager.h"
#include "vkEngine.h"
#include "vkDevice.h"
#include "vkLogger.h"
#include "vkMesh.h"

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
		std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts; // descriptor set layouts for some pipeline(shader)
		VkDescriptorSetLayout descriptorSetLayout;
		// creating descriptor set layout for default shader //////////////////////////////////////////////////
		std::array<VkDescriptorSetLayoutBinding, 2> defaultDescriptorSetLayoutBindings{};
		defaultDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		defaultDescriptorSetLayoutBindings[0].binding = 0;
		defaultDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		defaultDescriptorSetLayoutBindings[0].descriptorCount = 1;

		defaultDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		defaultDescriptorSetLayoutBindings[1].binding = 0;
		defaultDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		defaultDescriptorSetLayoutBindings[1].descriptorCount = 1;

		VkDescriptorSetLayoutCreateInfo defaultDescriptorLayoutCI{};
		defaultDescriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		defaultDescriptorLayoutCI.bindingCount = static_cast<uint32_t>(defaultDescriptorSetLayoutBindings.size());
		defaultDescriptorLayoutCI.pBindings = defaultDescriptorSetLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_LogicalDevice, &defaultDescriptorLayoutCI, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			vkLog->Log("Default descriptor set layput creation failed.");
		}
		vDescriptorSetLayouts.push_back(descriptorSetLayout);
		mDescriptorSetLayoutsMap[PL_DEFAULT] = vDescriptorSetLayouts;
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		return true;
	}

	std::vector<VkDescriptorSetLayout> vkPipelineManager::GetDescriptorSetLayouts(ePipeline pipeline)
	{
		if (mDescriptorSetLayoutsMap.find(pipeline) != mDescriptorSetLayoutsMap.end())
		{
			return mDescriptorSetLayoutsMap[pipeline];
		}
		return {}; // return empty vector
	}

	bool vkPipelineManager::CreatePipelineLayouts()
	{
		VkPipelineLayout pipelineLayout;
		
		// pipeline layout for default shader ////////////////////////////////////////////////////

		//const int nbDescriptorSetLayouts = mDescriptorSetLayoutsMap[PL_DEFAULT].size();
		//std::array<VkDescriptorSetLayout, 1> descriptorSetLayouts;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = mDescriptorSetLayoutsMap[PL_DEFAULT];
		
		VkPipelineLayoutCreateInfo pipelineLayoutCI{};
		pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCI.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutCI.pSetLayouts = descriptorSetLayouts.data();
		
		if (vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			vkLog->Log("Default shader pipeline layout creation failed.");
		}
		mPipelineLayoutMap[PL_DEFAULT] = pipelineLayout;
		//////////////////////////////////////////////////////////////////////////////////////////
	
		


		return true;
	}

	VkPipelineLayout vkPipelineManager::GetPipelineLayout(ePipeline pipeline)
	{
		if (mPipelineLayoutMap.find(pipeline) != mPipelineLayoutMap.end())
		{
			return mPipelineLayoutMap[pipeline];
		}
		return nullptr;
	}

	bool vkPipelineManager::CreateRenderPasses()
	{
		// creating render passes


		return true;
	}

	VkRenderPass vkPipelineManager::GetRenderPass(ePipeline pipeline)
	{
		VkRenderPass rPass = nullptr;

		return rPass;
	}

	bool vkPipelineManager::CreatePipelines()
	{
		////////////////creation of default pipeline /////////////////////////////////////////////////
		
		VkGraphicsPipelineCreateInfo vkGraphicsPipelineCI;

		typedef struct VkGraphicsPipelineCreateInfo {
			VkStructureType                                  sType;
			const void* pNext;
			VkPipelineCreateFlags                            flags;
			uint32_t                                         stageCount;
			const VkPipelineShaderStageCreateInfo* pStages;
			const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
			const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
			const VkPipelineTessellationStateCreateInfo* pTessellationState;
			const VkPipelineViewportStateCreateInfo* pViewportState;
			const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
			const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
			const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
			const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
			const VkPipelineDynamicStateCreateInfo* pDynamicState;
			VkPipelineLayout                                 layout;
			VkRenderPass                                     renderPass;
			uint32_t                                         subpass;
			VkPipeline                                       basePipelineHandle;
			int32_t                                          basePipelineIndex;
		} VkGraphicsPipelineCreateInfo;
		
		VkPipelineVertexInputStateCreateInfo vertexInputStateCI = vk::initializers::PipelineVertexInputStateCreateInfo(
			vkVertex::GetBindingDesc(), vkVertex::GetAttributeDesc());

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI = vk::initializers::PipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, false);

		const std::vector<VkViewport> viewPorts = {
			{0.0f, 0.0f,
			1.0f, 1.0f,
			0.1f, 1000.0f},
		};

		const std::vector<VkRect2D> scissors = {
			{0, 0},  //offset
			{1, 1}   //extent
		};


		VkPipelineViewportStateCreateInfo viewPortStateCI = vk::initializers::PipelineViewportStateCreateInfo(viewPorts, scissors);

		VkPipelineRasterizationStateCreateInfo rasterizationStateCI = vk::initializers::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = vk::initializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL); // disabling stencil test for now


		//////////////////////////////////////////////////////////////////////////////////////////////


		return true;
	}
}