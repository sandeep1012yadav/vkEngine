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
		m_vkLogicalDevice = pEngine->GetDevice()->GetLogicalDevice();

		bool bStatus = true;
		if (bStatus) {
			bStatus = CreateDescriptorSetLayouts();
			if (bStatus) {
				vkLog->Log("Descriptor set layouts created successfully...");
			}
		}
		if (bStatus) {
			bStatus = CreatePipelineLayouts();
			if (bStatus) {
				vkLog->Log("Pipeline layouts created successfully...");
			}
		}
		if (bStatus) {
			bStatus = CreateRenderPasses();
			if (bStatus) {
				vkLog->Log("Render Passes created successfully...");
			}
		}
		if (bStatus) {
			bStatus = CreatePipelines();
			if (bStatus) {
				vkLog->Log("Pipelines created successfully...");
			}
		}


	}

	vkPipelineManager::~vkPipelineManager()
	{

		// destroying render passess
		for (auto const& x : m_vkRenderPasses)
		{
			vkDestroyRenderPass(m_vkLogicalDevice, x.second, nullptr);
		}

		// destroying pipelines
		for (auto const& x : m_vkPipelineMap)
		{
			vkDestroyPipeline(m_vkLogicalDevice, x.second, nullptr);
		}

		// destroying descriptor set layouts
		for (auto const& x : m_vkDescriptorSetLayoutsMap)
		{
			for (const VkDescriptorSetLayout& y : x.second)
			{
				vkDestroyDescriptorSetLayout(m_vkLogicalDevice, y, nullptr);
			}
		}

		// destroying pipeline layout
		for (auto const& x : m_vkPipelineLayoutMap)
		{
			vkDestroyPipelineLayout(m_vkLogicalDevice, x.second, nullptr);
		}
	}

	bool vkPipelineManager::CreateDescriptorSetLayouts()
	{
		bool bResult = true;
		std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts; // descriptor set layouts for some pipeline(shader)
		VkDescriptorSetLayout descriptorSetLayout;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// creating descriptor set layouts for standard shader //////////////////////////////////////////////////
		{
			{
				std::array<VkDescriptorSetLayoutBinding, 1> descriptorSetLayoutBindings{};
				descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorSetLayoutBindings[0].binding = 0;
				descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				descriptorSetLayoutBindings[0].descriptorCount = 1;

				VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
				descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

				descriptorLayoutCI.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
				descriptorLayoutCI.pBindings = descriptorSetLayoutBindings.data();

				VkResult result = vkCreateDescriptorSetLayout(m_vkLogicalDevice, &descriptorLayoutCI, nullptr, &descriptorSetLayout);
				if (result != VK_SUCCESS) {
					bResult = false;
					vkLog->Log("Standard descriptor set layput creation failed.");
				}
				vDescriptorSetLayouts.push_back(descriptorSetLayout);
			}

			{
				std::array<VkDescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings{};
				descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorSetLayoutBindings[0].binding = 0;
				descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings[0].descriptorCount = 1;
				
				descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBindings[1].binding = 1;
				descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings[1].descriptorCount = 1;

				VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
				descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorLayoutCI.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
				descriptorLayoutCI.pBindings = descriptorSetLayoutBindings.data();

				VkResult result = vkCreateDescriptorSetLayout(m_vkLogicalDevice, &descriptorLayoutCI, nullptr, &descriptorSetLayout);
				if (result != VK_SUCCESS) {
					bResult = false;
					vkLog->Log("Standard descriptor set layput creation failed.");
				}
				vDescriptorSetLayouts.push_back(descriptorSetLayout);

				m_vkDescriptorSetLayoutsMap[ePipeline::PL_STANDARD] = vDescriptorSetLayouts;
			}
			vDescriptorSetLayouts.clear();
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////


		return bResult;
	}

	std::vector<VkDescriptorSetLayout> vkPipelineManager::GetDescriptorSetLayouts(ePipeline pipeline) const
	{
		auto it = m_vkDescriptorSetLayoutsMap.find(pipeline);
		if (it != m_vkDescriptorSetLayoutsMap.end())
		{
			return it->second;
		}
		return {}; // return empty vector
	}

	bool vkPipelineManager::CreatePipelineLayouts()
	{
		bool bResult = true;
		VkPipelineLayout pipelineLayout;

		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////pipeline layout for standard shader ///////////////////////////////////////////
		{
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts = m_vkDescriptorSetLayoutsMap[ePipeline::PL_STANDARD];

			VkPushConstantRange pushConstantRange = {};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Specify the shader stage
			pushConstantRange.offset = 0; // Offset within the push constant block
			pushConstantRange.size = sizeof(glm::mat4); // Size of the push constant block

			VkPipelineLayoutCreateInfo pipelineLayoutCI{};
			pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCI.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			pipelineLayoutCI.pSetLayouts = descriptorSetLayouts.data();
			pipelineLayoutCI.pushConstantRangeCount = 1;
			pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

			if (vkCreatePipelineLayout(m_vkLogicalDevice, &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				vkLog->Log("Default shader pipeline layout creation failed.");
				bResult = false;
			}
			m_vkPipelineLayoutMap[ePipeline::PL_STANDARD] = pipelineLayout;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////
	
		return bResult;
	}

	VkPipelineLayout vkPipelineManager::GetPipelineLayout(ePipeline pipeline) const
	{
		auto it = m_vkPipelineLayoutMap.find(pipeline);
		if (it != m_vkPipelineLayoutMap.end())
		{
			return it->second;
		}
		return VK_NULL_HANDLE;
	}

	bool vkPipelineManager::CreateRenderPasses()
	{
		bool bStatus = true;
		const VkSwapchainKHR& swapChain = m_pvkEngine->GetDevice()->GetSwapChain();
		VkRenderPass renderPass;

		// Forward_Rendering_Geometry render pass ////////////////////////////////////////////////
		std::array<VkAttachmentDescription, 2> attachments = {};
		// Color attachment
		attachments[0].format = m_pvkEngine->GetDevice()->GetSwapChainImageFormat();
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		attachments[1].format = m_pvkEngine->GetDevice()->GetDepthStencilFormat();
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		

		// Define the color attachment reference
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0; // Index of the color attachment in the attachment description array
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Define the depth attachment reference (optional)
		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1; // Index of the depth attachment in the attachment description array
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Define the subpass
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;
		subpass.pResolveAttachments = nullptr;
		
		// Subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dependencies[0].dependencyFlags = 0;

		dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].dstSubpass = 0;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].srcAccessMask = 0;
		dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependencies[1].dependencyFlags = 0;

		// Define the render pass create info
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size()); // Number of attachments (color + depth)
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		// Create the render pass
		if (vkCreateRenderPass(m_vkLogicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			vkLog->Log("Render Pass \"RP_Forward_Rendering_Geometry\" creation failed...");
			bStatus = false;
		}
		m_vkRenderPasses[eRenderPass::RP_Forward_Rendering_Geometry] = renderPass;
		//////////////////////////////////////////////////////////////////////////////////////////


		return bStatus;
	}

	VkRenderPass vkPipelineManager::GetRenderPass(eRenderPass renderPass) const
	{
		auto it = m_vkRenderPasses.find(renderPass);
		if (it != m_vkRenderPasses.end())
		{
			return it->second;
		}

		return VK_NULL_HANDLE;
	}

	bool vkPipelineManager::CreatePipelines()
	{
		bool bStatus = true;
		VkPipeline pipeline;

		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////creation of standard pipeline /////////////////////////////////////////////////
		{
			VkRenderPass renderPass = GetRenderPass(eRenderPass::RP_Forward_Rendering_Geometry);
			VkPipelineLayout pipelineLayout = GetPipelineLayout(ePipeline::PL_STANDARD);

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
			VkShaderModule vertexShaderModule = vk::tools::CreateShaderModule(m_vkLogicalDevice, "./resources/shaders/standard/standard_vert.spv");
			shaderStages.push_back(vk::initializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule, "main"));

			VkShaderModule fragShaderModule = vk::tools::CreateShaderModule(m_vkLogicalDevice, "./resources/shaders/standard/standard_frag.spv");
			shaderStages.push_back(vk::initializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule, "main"));

			std::vector<VkVertexInputBindingDescription> inputBindingDesc = vkVertex::GetBindingDesc();
			std::vector<VkVertexInputAttributeDescription> inputAttributeDesc = vkVertex::GetAttributeDesc();
			VkPipelineVertexInputStateCreateInfo vertexInputStateCI = vk::initializers::PipelineVertexInputStateCreateInfo(
				inputBindingDesc, inputAttributeDesc);

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

			VkPipelineRasterizationStateCreateInfo rasterizationStateCI = vk::initializers::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);

			VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = vk::initializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL); // disabling stencil test for now

			// Fixed-function stage: Multisampling
			VkPipelineMultisampleStateCreateInfo multisampling = vk::initializers::PipelineMultisampleStateCreateInfo(false, VK_SAMPLE_COUNT_1_BIT);

			// Fixed-function stage: Color blending
			VkPipelineColorBlendAttachmentState colorBlendAttachment = vk::initializers::PipelineColorBlendAttachmentState(VK_FALSE,
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

			VkPipelineColorBlendStateCreateInfo colorBlendStateCI = vk::initializers::PipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, 1, &colorBlendAttachment);

			std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, };

			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
			pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
			pipelineDynamicStateCreateInfo.flags = 0;

			VkGraphicsPipelineCreateInfo vkGraphicsPipelineCI = vk::initializers::GraphicsPipelineCreateInfo(
				static_cast<uint32_t>(shaderStages.size()), shaderStages.data(), &vertexInputStateCI, &inputAssemblyStateCI, nullptr,
				&viewPortStateCI, &rasterizationStateCI, nullptr, &depthStencilStateCI, &colorBlendStateCI, &pipelineDynamicStateCreateInfo, pipelineLayout, renderPass);

			VkResult result = vkCreateGraphicsPipelines(m_vkLogicalDevice, VK_NULL_HANDLE, 1, &vkGraphicsPipelineCI, nullptr, &pipeline);
			if (result != VK_SUCCESS) {
				vkLog->Log("Graphics pipeline \"Standard\" creation failed...");
				bStatus = false;
			}

			vkDestroyShaderModule(m_vkLogicalDevice, vertexShaderModule, nullptr);
			vkDestroyShaderModule(m_vkLogicalDevice, fragShaderModule, nullptr);

			m_vkPipelineMap[ePipeline::PL_STANDARD] = pipeline;
		}
		

		//////////////////////////////////////////////////////////////////////////////////////////////


		return bStatus;
	}

	VkPipeline vkPipelineManager::GetPipeline(ePipeline pipeline) const
	{
		auto it = m_vkPipelineMap.find(pipeline);
		if (it != m_vkPipelineMap.end())
		{
			return it->second;
		}
		return VK_NULL_HANDLE;
	}
}