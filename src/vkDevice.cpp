#include "vkDevice.h"
#include "vkLogger.h"
#include "vkEngine.h"
#include "vkWindow.h"
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
	}

	vkDevice::~vkDevice()
	{

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

		vkLog->Log("Supported device level extension properties :");
		for (const auto& extension : m_vkAvailableDeviceLevelExtentions) {
			vkLog->Log("	", extension.extensionName);
		}
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
		for (VkExtensionProperties ext : m_vkAvailableDeviceLevelExtentions)
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
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

		VkExtent2D swapChainExtent;
		if (m_vkSwapChainSupportDetails.surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			swapChainExtent = m_vkSwapChainSupportDetails.surfaceCapabilities.currentExtent;
		}
		else
		{
			int width, height;
			m_pvkEngine->GetWindow()->GetFrameBufferSize(&width, &height);
			swapChainExtent.height = static_cast<uint32_t>(height);
			swapChainExtent.width = static_cast<uint32_t>(width);

			swapChainExtent.height = std::clamp(swapChainExtent.height, m_vkSwapChainSupportDetails.surfaceCapabilities.minImageExtent.height,
				m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageExtent.height);
			swapChainExtent.width = std::clamp(swapChainExtent.width, m_vkSwapChainSupportDetails.surfaceCapabilities.minImageExtent.width,
				m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageExtent.width);
		}

		// setting the below swap chain modes
		m_vkCurrentPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		m_vkCurrentSurfaceFormat = { VK_FORMAT_B8G8R8A8_SRGB,  VK_COLORSPACE_SRGB_NONLINEAR_KHR};
		
		uint32_t imageCount = m_vkSwapChainSupportDetails.surfaceCapabilities.minImageCount + 1;
		if (m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageCount > 0 && 
			imageCount > m_vkSwapChainSupportDetails.surfaceCapabilities.maxImageCount)
		{
			imageCount = m_vkSwapChainSupportDetails.surfaceCapabilities.minImageCount;
		}

		VkSwapchainCreateInfoKHR vkSwapchainCreateInfo{};
		vkSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vkSwapchainCreateInfo.surface = m_pvkEngine->GetSurface();
		vkSwapchainCreateInfo.imageExtent = swapChainExtent;
		vkSwapchainCreateInfo.imageFormat = m_vkCurrentSurfaceFormat.format;
		vkSwapchainCreateInfo.imageColorSpace = m_vkCurrentSurfaceFormat.colorSpace;
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
		vkSwapchainCreateInfo.presentMode = m_vkCurrentPresentMode;
		vkSwapchainCreateInfo.clipped = VK_TRUE;
		vkSwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vkSwapchainCreateInfo.flags = VK_SWAPCHAIN_CREATE_PROTECTED_BIT_KHR;

		VkSwapchainKHR swapChain;
		if (vkCreateSwapchainKHR(m_vkDevice, &vkSwapchainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
		{
			return false;
		}
		vkLog->Log("Swap chain created successfully...");
		//m_vkSwapchain = swapChain;
		return true;
	}
}