#include "vkCore.h"
#include "vkLogger.h"

namespace vk
{
	namespace tools
	{
		VkShaderModule CreateShaderModule(
			const VkDevice& device,
			const std::string& spirName) 
		{
			size_t shaderSize;
			char* shaderCode{ nullptr };
			
			std::ifstream is(spirName, std::ios::binary | std::ios::in | std::ios::ate);

			if (is.is_open())
			{
				shaderSize = is.tellg();
				is.seekg(0, std::ios::beg);
				// Copy file contents into a buffer
				shaderCode = new char[shaderSize];
				is.read(shaderCode, shaderSize);
				is.close();
			}
			
			if (shaderCode)
			{
				// Create a new shader module that will be used for pipeline creation
				VkShaderModuleCreateInfo shaderModuleCI{};
				shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderModuleCI.codeSize = shaderSize;
				shaderModuleCI.pCode = (uint32_t*)shaderCode;

				VkShaderModule shaderModule;
				VkResult result = vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule);

				if (result != VK_SUCCESS) {
					vkLog->Log("Couldn't create shader module for ", spirName);
				}

				delete[] shaderCode;

				return shaderModule;
			}
			else
			{
				vkLog->Log("Error: Could not open shader file : ", spirName);
				return VK_NULL_HANDLE;
			}
		}

		void UpdateUniformBuffer(VkDevice device, VkDeviceMemory memory, const void* data, size_t size)
		{
			void* mappedMemory;
			vkMapMemory(device, memory, 0, size, 0, &mappedMemory);
			memcpy(mappedMemory, data, size);
			// If the memory property doesn't include VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			// you need to flush the mapped memory range to make it visible to the GPU
			vkUnmapMemory(device, memory);
		}

		void ChangeImageLayout(const VkCommandBuffer& cmdBuffer, VkImage& image, const VkImageLayout& oldImageLayout, 
			const VkImageLayout& newImageLayout, const VkImageSubresourceRange& subresourceRange, 
			const VkPipelineStageFlags& srcStageMask, const VkPipelineStageFlags& dstStageMask)
		{
			// Create an image barrier object
			VkImageMemoryBarrier imageMemoryBarrier = vk::initializers::ImageMemoryBarrier(image, oldImageLayout, newImageLayout, subresourceRange, 0, 0);

			// Source layouts (old)
			// Source access mask controls actions that have to be finished on the old layout
			// before it will be transitioned to the new layout
			switch (oldImageLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				// Image layout is undefined (or does not matter)
				// Only valid as initial layout
				// No flags required, listed only for completeness
				imageMemoryBarrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				// Image is preinitialized
				// Only valid as initial layout for linear images, preserves memory contents
				// Make sure host writes have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image is a color attachment
				// Make sure any writes to the color buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image is a depth/stencil attachment
				// Make sure any writes to the depth/stencil buffer have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image is a transfer source
				// Make sure any reads from the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image is a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image is read by a shader
				// Make sure any shader reads from the image have been finished
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				// Other source layouts aren't handled (yet)
				break;
			}

			// Target layouts (new)
			// Destination access mask controls the dependency for the new image layout
			switch (newImageLayout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image will be used as a transfer destination
				// Make sure any writes to the image have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image will be used as a transfer source
				// Make sure any reads from the image have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image will be used as a color attachment
				// Make sure any writes to the color buffer have been finished
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image layout will be used as a depth/stencil attachment
				// Make sure any writes to depth/stencil buffer have been finished
				imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image will be read in a shader (sampler, input attachment)
				// Make sure any writes to the image have been finished
				if (imageMemoryBarrier.srcAccessMask == 0)
				{
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				}
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				// Other source layouts aren't handled (yet)
				break;

				
			}

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(
				cmdBuffer,
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);
		}
	}
}