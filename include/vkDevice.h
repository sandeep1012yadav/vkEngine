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


		const VkDevice& GetLogicalDevice() { return m_vkDevice; }
		const VkSwapchainKHR& GetSwapChain() { return m_vkSwapchain; };
		const VkFormat& GetSwapChainImageFormat() { return m_vkSwapChainSurfaceFormat.format; }
		const VkFormat& GetDepthStencilFormat() { return m_vkDepthStencilBuffer.format; }
		const VkExtent2D& GetSwapChainExtent() { return m_vkSwapChainExtent; }
		const std::vector<VkImage>& GetSwapChainImages() { return m_vkSwapChainImages; }
		const std::vector<VkFramebuffer>& GetFrameBuffers() { return m_vkFrameBuffers; };
		const VkCommandBuffer& GetCommandBuffers() { return m_vkCommandBuffer; };
		const VkQueue& GetGraphicsQueue() { return m_vkGraphicsQueue; }
		const VkQueue& GetPresentQueue() { return m_vkPresentQueue; }

		uint32_t FindMemoryType(const VkMemoryRequirements& suitableMemRequirements, const VkMemoryPropertyFlags& preferredMemType);
		VkFormat GetSupportedDepthFormat();
		bool CreateFrameBuffers();
		bool CreateCommandBuffers();

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
		std::vector<VkImageView> m_vkSwapChainImageViews;
		std::vector<VkFramebuffer> m_vkFrameBuffers;
		VkExtent2D m_vkSwapChainExtent;

		DepthStencilBuffer m_vkDepthStencilBuffer;

		VkCommandPool m_vkCommandPool;
		VkCommandBuffer m_vkCommandBuffer;

		

		void PickPhysicalDevice();
		bool IsDeviceExtensionSupported(const std::string extension);
		bool CheckDeviceExtensionSupport(const VkPhysicalDevice physicalDevice);
		int  RateDeviceSuitability(const VkPhysicalDevice physicalDevice);
		const QueueFamilyIndices& FindQueueFamilies();
		bool IsDevieSuitable();

		bool CreateLogicalDevice();
		SwapChainSupportDetails QuerySwapChainSupportDetails();
		bool CreateSwapChain();
		
		VkResult CreateDepthStencilBuffer();
	};

	
}