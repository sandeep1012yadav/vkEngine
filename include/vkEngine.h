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
		~vkEngine();

	private:
		vkWindow* m_pvkWindow;
		vkDevice* m_pvkDevice;
		bool m_bEngineRunning;

		VkInstance m_vkInstance;

		bool InitializeWindow();

		bool InitializeVulkan();

		bool InitializeDevice();
	};
}