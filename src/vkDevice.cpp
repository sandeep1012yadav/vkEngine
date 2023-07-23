#include "vkDevice.h"
#include "vkLogger.h"
#include "vkEngine.h"
namespace vk
{
	vkDevice::vkDevice(const VkInstance vkInstance, const vkEngine* pEngine)
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
			if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				vkLog->Log(">>>>>>>>>>>>>>>>>>>>>This queue is graphics queue");
				m_vkQueueFamilyIndices.graphicsFamilyIndex = index;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, index, m_pvkEngine->GetSurface(), &presentSupport);
			if (presentSupport)
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
		
		// TBD 
		deviceCreateInfo.enabledExtensionCount = 0;
		//deviceCreateInfo.ppEnabledExtensionNames
		deviceCreateInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
}