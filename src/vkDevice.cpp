#include "vkDevice.h"
#include "vkLogger.h"

namespace vk
{
	vkDevice::vkDevice(const VkInstance vkInstance)
	{
		m_vkPhysicalDevice = VK_NULL_HANDLE;
		m_vkInstance = vkInstance;

		PickPhysicalDevice();

		FindQueueFamilies();
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

	void vkDevice::FindQueueFamilies()
	{
		uint32_t queueFamiliesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamiliesCount, nullptr);

		std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamiliesCount, vkQueueFamilyProperties.data());

		//std::string log = "";
		//log += queueFamiliesCount;
		vkLog->Log("Queue Families Count : ", queueFamiliesCount);

		int index = 0;
		for (const VkQueueFamilyProperties& queueFamilyProperties : vkQueueFamilyProperties)
		{

			//vkLog->Log("With Index : " + index);
			vkLog->Log("With index ", index, "   Queue Family has queues : ", queueFamilyProperties.queueCount);
			if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				vkLog->Log(">>>>>>>>>>>>>>>>>>>>>This queue is graphics queue");
			}
			index++;
		}
	}
}