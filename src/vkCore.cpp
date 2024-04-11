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
	}
}