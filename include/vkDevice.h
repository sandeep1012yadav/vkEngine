#pragma once
#include "vkCore.h"
namespace vk
{
	class vkEngine;
	struct vkVertex;
	class vkDevice
	{
	public:
		vkDevice(const VkInstance vkInstance, vkEngine* pEngine);
		~vkDevice();

		const VkDevice& GetLogicalDevice() const { return m_vkDevice; }
		const VkSwapchainKHR& GetSwapChain() const { return m_vkSwapchain; };
		const VkFormat& GetSwapChainImageFormat() const { return m_vkSwapChainSurfaceFormat.format; }
		const VkFormat& GetDepthStencilFormat() const { return m_vkDepthStencilBuffer.format; }
		const VkExtent2D& GetSwapChainExtent() const { return m_vkSwapChainExtent; }
		const std::vector<VkImage>& GetSwapChainImages() const { return m_vkSwapChainImages; }
		const std::vector<VkFramebuffer>& GetFrameBuffers() const { return m_vkFrameBuffers; };
		const VkCommandBuffer& GetCommandBuffers() const { return m_vkCommandBuffer; };
		const VkQueue& GetGraphicsQueue() const { return m_vkGraphicsQueue; }
		const VkQueue& GetPresentQueue() const { return m_vkPresentQueue; }

		uint32_t FindMemoryType(const VkMemoryRequirements& suitableMemRequirements, const VkMemoryPropertyFlags& preferredMemType) const;
		VkFormat GetSupportedDepthFormat() const;
		bool CreateFrameBuffers();
		bool CreateCommandBuffers();

		bool CopyBuffer(const VkBuffer& stagingBuffer, const VkBuffer& vertexBuffer, const uint32_t& bufferSize) const;
		bool CreateBuffer(const VkDeviceSize& bufferSize, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& preferredMemType, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
		bool CreateVertexBuffer(const uint32_t nbVertices, const vkVertex* vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory) const;
		bool CreateIndexBuffer(const uint32_t nbIndices, const uint32_t* indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory) const;

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