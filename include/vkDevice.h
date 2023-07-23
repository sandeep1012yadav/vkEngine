#pragma once
#include "vkCore.h"
namespace vk
{
	class vkEngine;
	class vkDevice
	{
	public:
		vkDevice(const VkInstance vkInstance, const vkEngine* pEngine);
		~vkDevice();

	private:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamilyIndex;
			std::optional<uint32_t> presentFamilyIndex;
		};
		const vkEngine* m_pvkEngine;
		VkInstance m_vkInstance;
		VkPhysicalDevice m_vkPhysicalDevice;
		VkPhysicalDeviceProperties m_vkPhysicalDeviceProperties;
		VkPhysicalDeviceFeatures m_vkPhysicalDeviceFeatures;
		VkDevice m_vkDevice;
		QueueFamilyIndices m_vkQueueFamilyIndices;
		VkQueue m_vkGraphicsQueue;
		VkQueue m_vkPresentQueue;


		void PickPhysicalDevice();
		
		bool CheckDeviceExtensionSupport(const VkPhysicalDevice physicalDevice);
		int  RateDeviceSuitability(const VkPhysicalDevice physicalDevice);
		const QueueFamilyIndices& FindQueueFamilies();
		bool IsDevieSuitable();

		bool CreateLogicalDevice();
	};

	
}