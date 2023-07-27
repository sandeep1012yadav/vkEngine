#pragma once
#include "vkCore.h"
namespace vk
{
	class vkEngine;
	class vkDevice
	{
	public:
		vkDevice(const VkInstance vkInstance, vkEngine* pEngine);
		~vkDevice();

	private:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamilyIndex;
			std::optional<uint32_t> presentFamilyIndex;
		};
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR surfaceCapabilities;
			std::vector<VkSurfaceFormatKHR> surfaceFormats;
			std::vector<VkPresentModeKHR> presentMode;
		};

		vkEngine* m_pvkEngine;
		VkInstance m_vkInstance;
		VkPhysicalDevice m_vkPhysicalDevice;
		VkPhysicalDeviceProperties m_vkPhysicalDeviceProperties;
		VkPhysicalDeviceFeatures m_vkPhysicalDeviceFeatures;
		VkDevice m_vkDevice;
		QueueFamilyIndices m_vkQueueFamilyIndices;
		VkQueue m_vkGraphicsQueue;
		VkQueue m_vkPresentQueue;
		std::vector<VkExtensionProperties> m_vkAvailableDeviceLevelExtentions;
		SwapChainSupportDetails m_vkSwapChainSupportDetails;
		VkSurfaceFormatKHR m_vkCurrentSurfaceFormat;
		VkPresentModeKHR m_vkCurrentPresentMode;
		VkSwapchainKHR m_vkSwapchain;


		void PickPhysicalDevice();
		bool IsDeviceExtensionSupported(const std::string extension);
		bool CheckDeviceExtensionSupport(const VkPhysicalDevice physicalDevice);
		int  RateDeviceSuitability(const VkPhysicalDevice physicalDevice);
		const QueueFamilyIndices& FindQueueFamilies();
		bool IsDevieSuitable();

		bool CreateLogicalDevice();
		SwapChainSupportDetails QuerySwapChainSupportDetails();
		bool CreateSwapChain();
	};

	
}