#pragma once
#include "vkCore.h"
namespace vk
{
	class vkDevice
	{
	public:
		vkDevice(const VkInstance vkInstance);
		~vkDevice();

	private:
		VkInstance m_vkInstance;
		VkPhysicalDevice m_vkPhysicalDevice;
		void PickPhysicalDevice();
		bool CheckDeviceExtensionSupport(const VkPhysicalDevice physicalDevice);
		int  RateDeviceSuitability(const VkPhysicalDevice physicalDevice);

		struct QueueFamilyIndicies
		{
			uint32_t graphicsFamilyIndex;
			uint32_t presentFamilyIndex;
		};

		void FindQueueFamilies();
	};

	
}