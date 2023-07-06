#pragma once

#include <vulkan/vulkan.h>

namespace vk
{
	class vkWindow;

	class vkEngine
	{
	public:
		vkEngine();
		
		bool StartEngine();
		bool StopEngine();
		~vkEngine();

	private:
		vkWindow* m_pvkWindow;
		bool m_bEngineRunning;

		VkInstance m_vkInstance;
		bool InitializeVulkan();
	};
}