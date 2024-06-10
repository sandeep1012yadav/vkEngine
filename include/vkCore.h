#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <array>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <optional>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <unordered_map>

namespace vk
{

	struct BufferDescriptorSetResource
	{
		VkDescriptorBufferInfo  bufferInfo = { VK_NULL_HANDLE , 0, 0 };
		VkDeviceMemory		    bufferMemory = VK_NULL_HANDLE;
	};

	struct ImageDescriptorSetResource
	{
		VkDescriptorImageInfo  imageInfo = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED };
		VkDeviceMemory         imageMemory = VK_NULL_HANDLE;
	};
	
	typedef struct _DepthStencilBuffer 
	{
		VkImage image;
		VkDeviceMemory memory;
		VkImageView view;
		VkFormat format;
	}DepthStencilBuffer;

	namespace tools
	{
		VkShaderModule CreateShaderModule(const VkDevice& device, const std::string& spirName);

		template<typename T>
		T* CopyVector(const std::vector<T>& vec)
		{
			T* arr = new T[vec.size()];
			std::copy(vec.begin(), vec.end(), arr);
			return arr;
		}

		void UpdateUniformBuffer(VkDevice device, VkDeviceMemory memory, const void* data, size_t size);

		void ChangeImageLayout(const VkCommandBuffer& cmdBuffer, VkImage& image, const VkImageLayout& oldImageLayout, 
			const VkImageLayout& newImageLayout, const VkImageSubresourceRange& subresourceRange, 
			const VkPipelineStageFlags& srcStageMask, const VkPipelineStageFlags& dstStageMask);
	};

	namespace initializers
	{
		inline VkBufferCreateInfo BufferCreateInfo(
			const VkDeviceSize& bufferSize,
			const VkBufferUsageFlags& bufferUsages,
			const VkSharingMode& sharingMode, 
			const uint32_t& queueFamilyIndexCount,
			const uint32_t* pQueueFamilyIndices)
		{
			VkBufferCreateInfo bufferCI = {};
			bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCI.size = bufferSize;
			bufferCI.usage = bufferUsages;
			bufferCI.sharingMode = sharingMode;
			bufferCI.queueFamilyIndexCount = queueFamilyIndexCount;
			bufferCI.pQueueFamilyIndices = pQueueFamilyIndices;

			return bufferCI;
		}

		inline VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(
			const std::vector<VkDescriptorPoolSize>& poolSizes,
			const uint32_t& maxSets)
		{
			VkDescriptorPoolCreateInfo poolCI{};
			poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolCI.pPoolSizes = poolSizes.data();
			poolCI.maxSets = maxSets;
			// Optionally, you can set the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag
			// to allow individual descriptor sets to be freed. By default, all sets are freed when
			// the pool is destroyed.
			// poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

			return poolCI;
		}

		inline VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(
			const VkDescriptorPool& descriptorPool,
			const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			allocInfo.pSetLayouts = descriptorSetLayouts.data();

			return allocInfo;
		}

		inline VkImageCreateInfo ImageCreateInfo(
			const VkImageType& imageType,
			const VkFormat& format,const VkExtent3D extent,
			const uint32_t mipLevels,
			const uint32_t arrayLayers,
			const VkSampleCountFlagBits& sampleCountFlagBits,
			const VkImageTiling& tiling,
			const VkImageUsageFlags& usages)
		{
			VkImageCreateInfo imageCI{};
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = imageType;
			imageCI.format = format;
			imageCI.extent = extent;
			imageCI.mipLevels = mipLevels;
			imageCI.arrayLayers = arrayLayers;
			imageCI.samples = sampleCountFlagBits;
			imageCI.tiling = tiling;
			imageCI.usage = usages;
			imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			return imageCI;
		}

		inline VkImageViewCreateInfo ImageViewCreateInfo(
			const VkImage& image,
			const VkImageViewType& viewType,
			const VkFormat& format,
			const VkImageAspectFlags& aspectMask, 
			const uint32_t baseMipLevel, 
			const uint32_t levelCount, 
			const uint32_t baseArrayLayer, 
			const uint32_t layerCount)
		{
			VkImageViewCreateInfo imageViewCI{};
			imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCI.image = image;
			imageViewCI.viewType = viewType;
			imageViewCI.format = format;
			imageViewCI.subresourceRange.aspectMask = aspectMask;
			imageViewCI.subresourceRange.baseMipLevel = baseMipLevel;
			imageViewCI.subresourceRange.levelCount = levelCount;
			imageViewCI.subresourceRange.baseArrayLayer = baseArrayLayer;
			imageViewCI.subresourceRange.layerCount = layerCount;

			return imageViewCI;
		}

		inline VkMemoryAllocateInfo MemoryAllocateInfo(const VkDeviceSize& deviceSize, uint32_t memTypeIndex)
		{
			VkMemoryAllocateInfo memoryAllocateInfo{};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = deviceSize;
			memoryAllocateInfo.memoryTypeIndex = memTypeIndex;
			return memoryAllocateInfo;
		}

		inline VkImageMemoryBarrier ImageMemoryBarrier(
			const VkImage& image,
			const VkImageLayout& oldImageLayout,
			const VkImageLayout& newImageLayout,
			const VkImageSubresourceRange& subresourceRange,
			const VkAccessFlags& srcAccessMask,
			const VkAccessFlags& dstAccessMask)
		{
			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.oldLayout = oldImageLayout;
			imageMemoryBarrier.newLayout = newImageLayout;
			imageMemoryBarrier.srcAccessMask = srcAccessMask;
			imageMemoryBarrier.dstAccessMask = dstAccessMask;
			imageMemoryBarrier.subresourceRange = subresourceRange;

			return imageMemoryBarrier;
		}

		inline VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(
			const VkShaderStageFlagBits& shaderStage,
			const VkShaderModule& module,
			const char* main)
		{
			VkPipelineShaderStageCreateInfo shaderStageCI = {};
			shaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCI.stage = shaderStage;
			shaderStageCI.pName = main;
			shaderStageCI.module = module;

			return shaderStageCI;
		}

		inline VkFramebufferCreateInfo FramebufferCreateInfo(
			const VkRenderPass& renderPass,
			const uint32_t& attachmentCount,
			const VkImageView* pAttachments,
			const uint32_t& width,
			const uint32_t& height,
			const uint32_t& layers)
		{
			VkFramebufferCreateInfo frameBufferCI{};
			frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCI.renderPass = renderPass;
			frameBufferCI.attachmentCount = attachmentCount;
			frameBufferCI.pAttachments = pAttachments;
			frameBufferCI.width = width;
			frameBufferCI.height = height;
			frameBufferCI.layers = layers;
			
			return frameBufferCI;
		}
		
		inline VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(
			const uint32_t& shaderStagesCount,
			const VkPipelineShaderStageCreateInfo* pStages,
			const VkPipelineVertexInputStateCreateInfo* pVertexInputState,
			const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState,
			const VkPipelineTessellationStateCreateInfo* pTessellationState,
			const VkPipelineViewportStateCreateInfo* pViewportState,
			const VkPipelineRasterizationStateCreateInfo* pRasterizationState,
			const VkPipelineMultisampleStateCreateInfo* pMultisampleState,
			const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState,
			const VkPipelineColorBlendStateCreateInfo* pColorBlendState,
			const VkPipelineDynamicStateCreateInfo* pDynamicState,
			const VkPipelineLayout& layout,
			const VkRenderPass& renderPass,
			const uint32_t& subpassIndex = 0,
			const VkPipelineCreateFlags& flags = 0,
			const VkPipeline& basePipelineHandle = VK_NULL_HANDLE,
			const int32_t& basePipelineIndex = -1)
		{
			VkGraphicsPipelineCreateInfo pipelineCI{};
			pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCI.stageCount = shaderStagesCount;
			pipelineCI.pStages = pStages;
			pipelineCI.pVertexInputState = pVertexInputState;
			pipelineCI.pInputAssemblyState = pInputAssemblyState;
			pipelineCI.pTessellationState = pTessellationState;
			pipelineCI.pViewportState = pViewportState;
			pipelineCI.pRasterizationState = pRasterizationState;
			pipelineCI.pMultisampleState = pMultisampleState;
			pipelineCI.pDepthStencilState = pDepthStencilState;
			pipelineCI.pColorBlendState = pColorBlendState;
			pipelineCI.pDynamicState = pDynamicState;
			pipelineCI.layout = layout;
			pipelineCI.renderPass = renderPass;
			pipelineCI.subpass = subpassIndex;
			pipelineCI.flags = flags;
			pipelineCI.basePipelineHandle = basePipelineHandle;
			pipelineCI.basePipelineIndex = basePipelineIndex;
			
			return pipelineCI;
		}

		inline VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo()
		{
			VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.basePipelineIndex = -1;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			return pipelineCreateInfo;
		}

		inline VkComputePipelineCreateInfo ComputePipelineCreateInfo(
			VkPipelineLayout layout,
			VkPipelineCreateFlags flags = 0)
		{
			VkComputePipelineCreateInfo computePipelineCreateInfo{};
			computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			computePipelineCreateInfo.layout = layout;
			computePipelineCreateInfo.flags = flags;
			return computePipelineCreateInfo;
		}

		inline VkVertexInputBindingDescription VertexInputBindingDescription(
			uint32_t binding,
			uint32_t stride,
			VkVertexInputRate inputRate)
		{
			VkVertexInputBindingDescription vInputBindDescription{};
			vInputBindDescription.binding = binding;
			vInputBindDescription.stride = stride;
			vInputBindDescription.inputRate = inputRate;
			return vInputBindDescription;
		}

		inline VkVertexInputAttributeDescription VertexInputAttributeDescription(
			uint32_t binding,
			uint32_t location,
			VkFormat format,
			uint32_t offset)
		{
			VkVertexInputAttributeDescription vInputAttribDescription{};
			vInputAttribDescription.location = location;
			vInputAttribDescription.binding = binding;
			vInputAttribDescription.format = format;
			vInputAttribDescription.offset = offset;
			return vInputAttribDescription;
		}

		inline VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo()
		{
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return pipelineVertexInputStateCreateInfo;
		}

		inline VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(
			const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions
		)
		{
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
			pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
			return pipelineVertexInputStateCreateInfo;
		}

		inline VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(
			VkPrimitiveTopology topology,
			VkPipelineInputAssemblyStateCreateFlags flags,
			VkBool32 primitiveRestartEnable)
		{
			VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
			pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipelineInputAssemblyStateCreateInfo.topology = topology;
			pipelineInputAssemblyStateCreateInfo.flags = flags;
			pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
			return pipelineInputAssemblyStateCreateInfo;
		}

		inline VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo(
			const std::vector<VkViewport>& viewPorts,
			const std::vector<VkRect2D>& scissors,
			VkPipelineViewportStateCreateFlags flags = 0)
		{
			VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
			pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipelineViewportStateCreateInfo.viewportCount = static_cast<uint32_t>(viewPorts.size());
			pipelineViewportStateCreateInfo.pViewports = viewPorts.data();
			pipelineViewportStateCreateInfo.scissorCount = static_cast<uint32_t>(scissors.size());
			pipelineViewportStateCreateInfo.pScissors = scissors.data();
			pipelineViewportStateCreateInfo.flags = flags;
			return pipelineViewportStateCreateInfo;
		}

		inline VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(
			VkPolygonMode polygonMode,
			VkCullModeFlags cullMode,
			VkFrontFace frontFace,
			VkPipelineRasterizationStateCreateFlags flags = 0)
		{
			VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
			pipelineRasterizationStateCreateInfo.cullMode = cullMode;
			pipelineRasterizationStateCreateInfo.frontFace = frontFace;
			pipelineRasterizationStateCreateInfo.flags = flags;
			pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
			return pipelineRasterizationStateCreateInfo;
		}

		inline VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(
			VkBool32 depthTestEnable,
			VkBool32 depthWriteEnable,
			VkCompareOp depthCompareOp)
		{
			VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
			pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
			pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
			pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
			pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			return pipelineDepthStencilStateCreateInfo;
		}

		inline VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(
			const VkBool32& sampleShadingEnable,
			const VkSampleCountFlagBits& rasterizationSamples)
		{
			VkPipelineMultisampleStateCreateInfo multiSampleStateCI{};
			multiSampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multiSampleStateCI.sampleShadingEnable = sampleShadingEnable;
			multiSampleStateCI.rasterizationSamples = rasterizationSamples;

			return multiSampleStateCI;
		}

		inline VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(
			const VkBool32& attachmentBlendEnable,
			const VkColorComponentFlags& colorWriteMask)
		{
			VkPipelineColorBlendAttachmentState colarBlendAttachmentState{};
			colarBlendAttachmentState.blendEnable = attachmentBlendEnable;
			colarBlendAttachmentState.colorWriteMask = colorWriteMask;

			return colarBlendAttachmentState;
		}

		inline VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(
			const VkBool32& logicOpEnable,
			const VkLogicOp& logicOp,
			const uint32_t& blendAttachmentCount, 
			const VkPipelineColorBlendAttachmentState* pBlendAttachments)
		{
			VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
			colorBlendStateCI.logicOpEnable = logicOpEnable;
			colorBlendStateCI.logicOp = logicOp;
			colorBlendStateCI.attachmentCount = blendAttachmentCount;
			colorBlendStateCI.pAttachments = pBlendAttachments;

			return colorBlendStateCI;
		}

		inline VkCommandPoolCreateInfo CommandPoolCreateInfo(
			const VkCommandPoolCreateFlags& flags,
			const uint32_t& queueFamilyIndex)
		{
			VkCommandPoolCreateInfo commandPoolCI{};
			commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCI.flags = flags;
			commandPoolCI.queueFamilyIndex = queueFamilyIndex;

			return commandPoolCI;
		}

		inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
			const VkCommandPool& commandPool,
			const VkCommandBufferLevel& level,
			const uint32_t& commandBufferCount)
		{
			VkCommandBufferAllocateInfo commandBufferAI{};
			commandBufferAI.commandPool = commandPool;
			commandBufferAI.level = level;
			commandBufferAI.commandBufferCount = commandBufferCount;

			return commandBufferAI;
		}

		inline VkFenceCreateInfo FenceCreateInfo(const VkFenceCreateFlags& flag)
		{
			VkFenceCreateInfo fenceCI = {};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCI.pNext = NULL;
			fenceCI.flags = flag;

			return fenceCI;
		}

		inline VkSemaphoreCreateInfo SemaphoreCreateInfo()
		{
			VkSemaphoreCreateInfo semaphoreCI = {};
			semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCI.pNext = NULL;
			semaphoreCI.flags = 0; // Reserved for future use

			return semaphoreCI;
		}

		inline VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferUsageFlags& flag)
		{
			VkCommandBufferBeginInfo commandBufferBI = {};
			commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBI.flags = flag;

			return commandBufferBI;
		}

		inline VkRenderPassBeginInfo RenderPassBeginInfo(
			const VkRenderPass& renderPass,
			const VkRect2D& renderArea,
			const uint32_t& clearValueCount,
			const VkClearValue clearValues[],
			const VkFramebuffer& frameBuffer)
		{
			VkRenderPassBeginInfo renderPassBI = {};
			renderPassBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBI.renderPass = renderPass;
			renderPassBI.renderArea = renderArea;
			renderPassBI.clearValueCount = clearValueCount;
			renderPassBI.pClearValues = clearValues;//
			renderPassBI.framebuffer = frameBuffer;

			return renderPassBI;
		}

		inline VkSubmitInfo SubmitInfo(const std::vector<VkCommandBuffer>& vCommandBuffers,
			const std::vector<VkSemaphore>& vWaitSemaphores,
			const std::vector<VkPipelineStageFlags> vWaitDstStageMask,
			const std::vector<VkSemaphore>& vSignalSemaphores)
		{
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = static_cast<uint32_t>(vCommandBuffers.size());
			submitInfo.pCommandBuffers = vCommandBuffers.data();
			submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vWaitSemaphores.size());
			submitInfo.pWaitSemaphores = vWaitSemaphores.data();
			submitInfo.pWaitDstStageMask = vWaitDstStageMask.data();
			submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vSignalSemaphores.size());
			submitInfo.pSignalSemaphores = vSignalSemaphores.data();

			return submitInfo;
		}

		inline VkPresentInfoKHR PresentInfoKHR(const uint32_t& swapchainImageIndex,
			const std::vector<VkSemaphore>& vWaitSemaphores,
			const std::vector<VkSwapchainKHR>& vSwapchains)
		{
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = static_cast<uint32_t>(vWaitSemaphores.size());
			presentInfo.pWaitSemaphores = vWaitSemaphores.data();
			presentInfo.swapchainCount = static_cast<uint32_t>(vSwapchains.size());
			presentInfo.pSwapchains = vSwapchains.data();
			presentInfo.pImageIndices = &swapchainImageIndex;

			return presentInfo;
		}

	}
}