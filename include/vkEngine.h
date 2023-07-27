#pragma once

#include "vkCore.h"

namespace vk
{
	class vkWindow;
	class vkDevice;

	class vkEngine
	{
	public:
		vkEngine();
		
		bool StartEngine();
		bool StopEngine();

		vkWindow* GetWindow();
		const vkDevice* GetDevice() const;
		const VkInstance& GetInstance() const;
		const VkSurfaceKHR& GetSurface() const;
		~vkEngine();

	private:
		vkWindow* m_pvkWindow;
		vkDevice* m_pvkDevice;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_vkSurface;
		bool m_bEngineRunning;

		bool InitializeWindow();
		bool InitializeVulkan();
		bool CreateSurface();
		bool InitializeDevice();
	};
}