#pragma once
#include "vkCore.h"
namespace vk
{
	class vkEngine;
	class vkDevice
	{
	public:
		vkDevice(const VkInstance vkInstance, vkEngine* pEngine);
		VkDevice GetLogicalDevice() { return m_vkDevice; }
		VkSwapchainKHR& GetSwapChain();
		VkFormat GetSwapChainImageFormat();
		VkExtent2D GetSwapChainExtent();
		std::vector<VkImage>& GetSwapChainImages();
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
		VkSurfaceFormatKHR m_vkSwapChainSurfaceFormat;
		VkPresentModeKHR m_vkSwapChainPresentMode;
		VkSwapchainKHR m_vkSwapchain;
		std::vector<VkImage> m_vkSwapChainImages;
		VkExtent2D m_vkSwapChainExtent;


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