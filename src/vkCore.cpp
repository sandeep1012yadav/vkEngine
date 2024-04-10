#include "vkCore.h"
#include "vkLogger.h"

namespace vk
{
	namespace tools
	{
		std::vector<uint32_t> LoadSPIRV(const std::string& filename)
		{
			// Open the file at the end to get the size
			std::ifstream file(filename, std::ios::ate | std::ios::binary);

			if (!file.is_open()) {
				throw std::runtime_error("failed to open file!");
			}

			// Get the size of the file and allocate a buffer
			size_t fileSize = (size_t)file.tellg();
			std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

			// Go back to the beginning of the file and read all the bytes at once
			file.seekg(0);
			file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

			// Check if the read was successful
			if (!file) {
				throw std::runtime_error("failed to read the SPIR-V file!");
			}

			file.close();

			return buffer;
		}

		VkShaderModule CreateShaderModule(
			const VkDevice& device,
			const std::string& spirName) 
		{
			std::vector<uint32_t> code = LoadSPIRV(spirName);
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = code.data();

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
			{
				vkLog->Log("Couldn't create shader module for ", spirName);
			}

			return shaderModule;
		}
	}
}