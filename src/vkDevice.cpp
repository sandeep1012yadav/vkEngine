#include "vkCore.h"
#include "vkDevice.h"
#include "vkLogger.h"
#include "vkEngine.h"
#include "vkWindow.h"
#include "vkPipelineManager.h"
namespace vk
{
	vkDevice::vkDevice(const VkInstance vkInstance, vkEngine* pEngine)
	{
		m_vkPhysicalDevice = VK_NULL_HANDLE;
		m_vkInstance = vkInstance;
		m_pvkEngine = pEngine;
		PickPhysicalDevice();

		if (!IsDevieSuitable())
		{
			vkLog->Log("Physical Device is not suitable...");
			return;
		}
		if (!CreateLogicalDevice())
		{
			vkLog->Log("Not able to create logical device...");
			return;
		}
		if (!CreateSwapChain())
		{
			vkLog->Log("Not able to create swapchain...");
			return;
		}
		if (CreateDepthStencilBuffer() != VK_SUCCESS)
		{
			vkLog->Log("Not able to create depth stencil buffer...");
			return;
		}
	}

	vkDevice::~vkDevice()
	{
		vkDestroySwapchainKHR(m_vkDevice, m_vkSwapchain, nullptr);


		// cleaning depth stencil buffer
		vkDestroyImageView(m_vkDevice, m_vkDepthStencilBuffer.view, nullptr);
		vkFreeMemory(m_vkDevice, m_vkDepthStencilBuffer.memory, nullptr);
		vkDestroyImage(m_vkDevice, m_vkDepthStencilBuffer.image, nullptr);
	}

	void vkDevice::PickPhysicalDevice()
	{
		vkLog->Log("Picking physical device ...   ");
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
		if (deviceCount == 0)
		{
			vkLog->Log("Failed to find any physical device");
			return;
		}

		std::vector<VkPhysicalDevice> availableDevices(deviceCount);
		vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, availableDevices.data());

		VkPhysicalDevice physicalDeviceWithMaxScore = VK_NULL_HANDLE;
		int maxDeviceScore = -100;
		for (VkPhysicalDevice device : availableDevices)
		{
			int deviceScore = RateDeviceSuitability(device);
			if (deviceScore > maxDeviceScore)
			{
				maxDeviceScore = deviceScore;
				physicalDeviceWithMaxScore = device;
			}
		}

		m_vkPhysicalDevice = physicalDeviceWithMaxScore;

		// fetching available device extensions 
		uint32_t deviceLevelExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(m_vkPhysicalDevice, nullptr, &deviceLevelExtensionCount, nullptr);
		m_vkAvailableDeviceLevelExtentions.resize(deviceLevelExtensionCount);
		vkEnumerateDeviceExtensionProperties(m_vkPhysicalDevice, nullptr, &deviceLevelExtensionCount, m_vkAvailableDeviceLevelExtentions.data());

		/*vkLog->Log("Supported device level extension properties :");
		for (const auto& extension : m_vkAvailableDeviceLevelExtentions) {
			vkLog->Log("	", extension.extensionName);
		}*/
	}

	bool vkDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice physicalDevice)
	{
		return true;
	}

	int vkDevice::RateDeviceSuitability(const VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
		int score = 0;
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		if (deviceFeatures.geometryShader == VK_FALSE)
			score = 0;

		return score;
	}

	const vkDevice::QueueFamilyIndices& vkDevice::FindQueueFamilies()
	{
		uint32_t queueFamiliesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamiliesCount, nullptr);

		std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamiliesCount, vkQueueFamilyProperties.data());

		vkLog->Log("Queue Families Count : ", queueFamiliesCount);

		int index = 0;
		for (const VkQueueFamilyProperties& queueFamilyProperties : vkQueueFamilyProperties)
		{

			vkLog->Log("Queue Family with index : \"", index,"\" has queues count : ", queueFamilyProperties.queueCount);
			if (!m_vkQueueFamilyIndices.graphicsFamilyIndex.has_value() && queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				vkLog->Log(">>>>>>>>>>>>>>>>>>>>>This queue is graphics queue");
				m_vkQueueFamilyIndices.graphicsFamilyIndex = index;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, index, m_pvkEngine->GetSurface(), &presentSupport);
			if (!m_vkQueueFamilyIndices.presentFamilyIndex.has_value() && presentSupport)
			{
				vkLog->Log(">>>>>>>>>>>>>>>>>>>>>This queue is present queue");
				m_vkQueueFamilyIndices.presentFamilyIndex = index;
			}

			index++;
		}

		return m_vkQueueFamilyIndices;
	}

	bool vkDevice::IsDevieSuitable()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies();
		if (queueFamilyIndices.graphicsFamilyIndex.has_value() && queueFamilyIndices.presentFamilyIndex.has_value())
		{
			vkLog->Log("Physical device is capable of creating graphics and presenting Queues...");
			return true;
		}
		return false;
	}

	bool vkDevice::IsDeviceExtensionSupported(const std::string extension)
	{
		for (const VkExtensionProperties& ext : m_vkAvailableDeviceLevelExtentions)
		{
			if (!strcmp(ext.extensionName, extension.c_str()))
			{
				return true;
			}
		}
		return false;
	}

	bool vkDevice::CreateLogicalDevice()
	{
		vkGetPhysicalDeviceFeatures(m_vkPhysicalDevice, &m_vkPhysicalDeviceFeatures);
		vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceProperties);

		std::vector<VkDeviceQueueCreateInfo> vQueueCreateInfos;
		std::set<uint32_t> uniqueQueueIndices = { m_vkQueueFamilyIndices.graphicsFamilyIndex.value(),
												  m_vkQueueFamilyIndices.presentFamilyIndex.value() };

		float queuePriority = 1.0f;
		for (uint32_t uniqueQueueIndex : uniqueQueueIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = uniqueQueueIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			vQueueCreateInfos.push_back(queueCreateInfo);
		}
		

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		//deviceCreateInfo.flags = 
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(vQueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = vQueueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &m_vkPhysicalDeviceFeatures;
		
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			//"VK_EXT_graphics_pipeline_library",
			//"VK_EXT_shader_object"

		};
		// TBD if we need to add more extensions  
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());;
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		
		deviceCreateInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice) != VK_SUCCESS)
		{
			return false;
		}

		vkGetDeviceQueue(m_vkDevice, m_vkQueueFamilyIndices.graphicsFamilyIndex.value(), 0, &m_vkGraphicsQueue);
		vkGetDeviceQueue(m_vkDevice, m_vkQueueFamilyIndices.presentFamilyIndex.value(), 0, &m_vkPresentQueue);

		

		return true;
	}

	vkDevice::SwapChainSupportDetails vkDevice::QuerySwapChainSupportDetails()
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, m_pvkEngine->GetSurface(), &details.surfaceCapabilities);
		uint32_t surfaceFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, m_pvkEngine->GetSurface(), &surfaceFormatCount, nullptr);
		if (surfaceFormatCount != 0)
		{
			details.surfaceFormats.resize(surfaceFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, m_pvkEngine->GetSurface(), &surfaceFormatCount, details.surfaceFormats.data());
		}
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, m_pvkEngine->GetSurface(), &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentMode.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, m_pvkEngine->GetSurface(), &presentModeCount, details.presentMode.data());
		}
		return details;
	}

	bool vkDevice::CreateSwapChain()
	{
		vkLog->Log("************Started creating swapchain***********************");
		if (!IsDeviceExtensionSupported(std::string("VK_KHR_swapchain")))
		{
			vkLog->Log("Swap chain device extension is not supported on the device...");
			return false;
		}
		vkLog->Log("Swap chain device extension is supported on the device...");

		m_vkSwapChainSupportDetails = QuerySwapChainSupportDetails();
		if (m_vkSwapChainSupportDetails.presentMode.empty() || m_vkSwapChainSupportDetails.surfaceFormats.empty())
		{
			vkLog->Log("Swap chain details are not sufficient to create swapchain...");
		}

		
		if (m_vkSwapChainSupportDetails.surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_vkSwapChainExtent = m_vkSwapChainSupportDetails.surfaceCapabilities.currentExtent;
		}
		else
		{
			int width, height;
			m_pvkEngine->GetWindow()->GetFrameBufferSize(&width, &height);
			m_vkSwapChainExtent.height = static_cast<uint32_t>(height);
			m_vkSwapChainExtent.width = static_cast<uint32_t>(width);

			m_vkSwapChainExtent.height = std::clamp(m_vkSwapChainExtent.height, m_vkSwapChainSupportDetails.surfaceCapabilities.minImageExtent.height,
				m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageExtent.height);
			m_vkSwapChainExtent.width = std::clamp(m_vkSwapChainExtent.width, m_vkSwapChainSupportDetails.surfaceCapabilities.minImageExtent.width,
				m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageExtent.width);
		}

		vkLog->Log("minImageCount in surface capabilities : ", m_vkSwapChainSupportDetails.surfaceCapabilities.minImageCount);
		vkLog->Log("maxImageCount in surface capabilities : ", m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageCount);

		// setting the below swap chain modes
		m_vkSwapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;  // basic mode available
		for (uint32_t presentModeIndex = 0; presentModeIndex < m_vkSwapChainSupportDetails.presentMode.size(); presentModeIndex++)
		{
			if (m_vkSwapChainSupportDetails.presentMode[presentModeIndex] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_vkSwapChainPresentMode = m_vkSwapChainSupportDetails.presentMode[presentModeIndex];
				break;
			}
			if (m_vkSwapChainSupportDetails.presentMode[presentModeIndex] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				m_vkSwapChainPresentMode = m_vkSwapChainSupportDetails.presentMode[presentModeIndex];
			}
		}

		//choosing formats for swapchain
		std::vector<VkFormat> preferredFormats = {
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_B8G8R8A8_SRGB,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_A8B8G8R8_UNORM_PACK32
		};

		bool bPreferredFormatFound = false;
		m_vkSwapChainSurfaceFormat = m_vkSwapChainSupportDetails.surfaceFormats[0];
		for (uint32_t formatIndex = 0; formatIndex < preferredFormats.size(); formatIndex++)
		{
			VkFormat preferredFormat = preferredFormats[formatIndex];
			for (uint32_t availableFormatIndex = 0; availableFormatIndex < m_vkSwapChainSupportDetails.surfaceFormats.size(); availableFormatIndex++)
			{
				if (preferredFormat == m_vkSwapChainSupportDetails.surfaceFormats[availableFormatIndex].format)
				{
					m_vkSwapChainSurfaceFormat = m_vkSwapChainSupportDetails.surfaceFormats[availableFormatIndex];
					bPreferredFormatFound = true;
					break;
				}
				if (bPreferredFormatFound)
					break;
			}
		}

		uint32_t imageCount = m_vkSwapChainSupportDetails.surfaceCapabilities.minImageCount + 1;
		if (m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageCount > 0 && 
			imageCount > m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageCount)
		{
			imageCount = m_vkSwapChainSupportDetails.surfaceCapabilities.minImageCount;
		}

		VkSwapchainCreateInfoKHR vkSwapchainCreateInfo{};
		vkSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vkSwapchainCreateInfo.surface = m_pvkEngine->GetSurface();
		vkSwapchainCreateInfo.imageExtent = m_vkSwapChainExtent;
		vkSwapchainCreateInfo.imageFormat = m_vkSwapChainSurfaceFormat.format;
		vkSwapchainCreateInfo.imageColorSpace = m_vkSwapChainSurfaceFormat.colorSpace;
		vkSwapchainCreateInfo.minImageCount = imageCount;
		vkSwapchainCreateInfo.imageArrayLayers = 1;
		vkSwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t queueFamilyIndices[] = { m_vkQueueFamilyIndices.graphicsFamilyIndex.value(), m_vkQueueFamilyIndices.presentFamilyIndex.value() };
		
		if (m_vkQueueFamilyIndices.graphicsFamilyIndex.value() != m_vkQueueFamilyIndices.presentFamilyIndex.value())
		{
			vkSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // image can be shared across multiple queue families without explicit ownership transfers.
			vkSwapchainCreateInfo.queueFamilyIndexCount = 2;
			vkSwapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			vkSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;  // an image will be owned by one queue at a time.
			vkSwapchainCreateInfo.queueFamilyIndexCount = 0;
			vkSwapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		vkSwapchainCreateInfo.preTransform = m_vkSwapChainSupportDetails.surfaceCapabilities.currentTransform;
		vkSwapchainCreateInfo.presentMode = m_vkSwapChainPresentMode;
		vkSwapchainCreateInfo.clipped = VK_TRUE;
		vkSwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vkSwapchainCreateInfo.flags = 0;// VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR;

		if (vkCreateSwapchainKHR(m_vkDevice, &vkSwapchainCreateInfo, nullptr, &m_vkSwapchain) != VK_SUCCESS)
		{
			return false;
		}
		// retrieving handles to images of swapchain 

		vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &imageCount, nullptr);
		m_vkSwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &imageCount, m_vkSwapChainImages.data());

		vkLog->Log("created swapchain images : ", imageCount);

		vkLog->Log("Swap chain created successfully...");
		return true;
	}

	bool vkDevice::CreateFrameBuffers()
	{
		bool bStatus = true;
		m_vkFrameBuffers.resize(m_vkSwapChainImages.size());
		m_vkSwapChainImageViews.resize(m_vkSwapChainImages.size());

		vkPipelineManager* pipelineManager = m_pvkEngine->GetPipelineManager();
		VkRenderPass renderPass = pipelineManager->GetRenderPass(eRenderPass::RP_Forward_Rendering_Geometry);

		for (uint32_t frameBufferIndex = 0; frameBufferIndex < m_vkFrameBuffers.size(); frameBufferIndex++)
		{
			VkImageViewCreateInfo imageViewCI = vk::initializers::ImageViewCreateInfo(
				m_vkSwapChainImages[frameBufferIndex], VK_IMAGE_VIEW_TYPE_2D, m_vkSwapChainSurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);

			VkResult result = vkCreateImageView(m_vkDevice, &imageViewCI, nullptr, &m_vkSwapChainImageViews[frameBufferIndex]);
			if (result != VK_SUCCESS) {
				vkLog->Log("ImageView creation from Swapchain Image failed ");
				bStatus = false;
				break;
			}
			
			std::vector<VkImageView> attachments;
			attachments.push_back(m_vkSwapChainImageViews[frameBufferIndex]);
			attachments.push_back(m_vkDepthStencilBuffer.view);
			VkFramebufferCreateInfo frameBufferCI = vk::initializers::FramebufferCreateInfo(renderPass, static_cast<uint32_t>(attachments.size()), 
				attachments.data(), m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, 1);

			result = vkCreateFramebuffer(m_vkDevice, &frameBufferCI, nullptr, &m_vkFrameBuffers[frameBufferIndex]);
			if (result != VK_SUCCESS) {
				vkLog->Log("Frame buffer creation from Swapchain Image failed ");
				bStatus = false;
				break;
			}
		}

		vkLog->Log("Frame buffers created successfully...");
		return bStatus;
	}

	const VkSwapchainKHR& vkDevice::GetSwapChain()
	{
		return m_vkSwapchain;
	}

	std::vector<VkImage>& vkDevice::GetSwapChainImages()
	{
		return m_vkSwapChainImages;
	}

	VkFormat vkDevice::GetSwapChainImageFormat()
	{
		return m_vkSwapChainSurfaceFormat.format;
	}

	VkFormat vkDevice::GetDepthStencilFormat()
	{
		return m_vkDepthStencilBuffer.format;
	}

	VkExtent2D vkDevice::GetSwapChainExtent()
	{
		return m_vkSwapChainExtent;
	}

	VkFormat vkDevice::GetSupportedDepthFormat()
	{
		// Since all depth formats may be optional, we need to find a suitable depth format to use
		// Start with the highest precision packed format
		std::vector<VkFormat> formatList = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : formatList)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(m_vkPhysicalDevice, format, &formatProps);
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				return format;
			}
		}

		return VK_FORMAT_UNDEFINED;
	}

	uint32_t vkDevice::FindMemoryType(const VkMemoryRequirements& suitableMemRequirements, const VkMemoryPropertyFlags& preferredMemType)
	{
		uint32_t selectedMemoryTypeForResource = 0;
		VkPhysicalDeviceMemoryProperties systemMemProp;
		vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &systemMemProp);

		for (uint32_t memIndex = 0; memIndex < systemMemProp.memoryTypeCount; memIndex++)
		{
			if (suitableMemRequirements.memoryTypeBits & (1 << memIndex) && systemMemProp.memoryTypes[memIndex].propertyFlags & preferredMemType)
			{
				selectedMemoryTypeForResource = memIndex;
			}
		}

		return selectedMemoryTypeForResource;
	}

	VkResult vkDevice::CreateDepthStencilBuffer()
	{
		vkLog->Log("\nCreating depth stencil buffer ...");
		VkResult result;
		m_vkDepthStencilBuffer.format = GetSupportedDepthFormat();
		if (m_vkDepthStencilBuffer.format == VK_FORMAT_UNDEFINED){
			vkLog->Log("Couldn't find the suitable depth format supported on the device");
		}

		VkImageCreateInfo imageCI = vk::initializers::ImageCreateInfo(VK_IMAGE_TYPE_2D, m_vkDepthStencilBuffer.format, { m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, 1 },
			1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		result = vkCreateImage(m_vkDevice, &imageCI, nullptr, &m_vkDepthStencilBuffer.image);
		if (result != VK_SUCCESS) {
			vkLog->Log("Creation of image for depth stencil buffer failed.");
		}

		VkMemoryRequirements memReqs{};
		vkGetImageMemoryRequirements(m_vkDevice, m_vkDepthStencilBuffer.image, &memReqs);
		uint32_t preferredMemTypeIndex = FindMemoryType(memReqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (preferredMemTypeIndex == 0) {
			vkLog->Log("Preferred memory type is not found.");
		}

		VkMemoryAllocateInfo memAllocInfo = vk::initializers::MemoryAllocateInfo(memReqs.size, preferredMemTypeIndex);
		result = vkAllocateMemory(m_vkDevice, &memAllocInfo, nullptr, &m_vkDepthStencilBuffer.memory);
		if (result != VK_SUCCESS) {
			vkLog->Log("Memory is not allocated for depth stencil buffer");
		}

		result = vkBindImageMemory(m_vkDevice, m_vkDepthStencilBuffer.image, m_vkDepthStencilBuffer.memory, 0);

		// Create the depth image view
		VkImageAspectFlags aspect = (m_vkDepthStencilBuffer.format >= VK_FORMAT_D16_UNORM_S8_UINT) ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
		VkImageViewCreateInfo imageViewCI = vk::initializers::ImageViewCreateInfo(m_vkDepthStencilBuffer.image, VK_IMAGE_VIEW_TYPE_2D, m_vkDepthStencilBuffer.format, aspect, 0, 1, 0, 1);
		
		result = vkCreateImageView(m_vkDevice, &imageViewCI, nullptr, &m_vkDepthStencilBuffer.view);
		
		vkLog->Log("Depth stencil buffer created ...");
		return result;
	}

	bool vkDevice::CreateCommandBuffers()
	{
		bool bStatus = true;
		VkCommandPoolCreateInfo commandPoolCI{};
		commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCI.queueFamilyIndex = m_vkQueueFamilyIndices.graphicsFamilyIndex.value();
		commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		return bStatus;
	}
}