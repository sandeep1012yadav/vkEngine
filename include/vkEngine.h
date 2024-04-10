#pragma once

#include "vkCore.h"
#include "vkLogger.h"
namespace vk
{
	class vkWindow;
	class vkDevice;
	class vkPipelineManager;
	class vkEngine
	{
	public:
		vkEngine();
		
		bool StartEngine();
		bool StopEngine();

		vkWindow* GetWindow();
		vkDevice* GetDevice();
		const VkInstance& GetInstance() const;
		const VkSurfaceKHR& GetSurface() const;
		~vkEngine();

	private:
		vkWindow* m_pvkWindow;
		vkDevice* m_pvkDevice;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_vkSurface;
		bool m_bEngineRunning;
		vkPipelineManager* m_pvkPipelineManager;

		bool InitializeWindow();
		bool InitializeVulkan();
		bool CreateSurface();
		bool InitializeDevice();
		bool InitializePipelineManager();
	};
}