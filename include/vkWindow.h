#pragma once
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include <string>
namespace vk
{
	class vkEngine;
	class vkWindow
	{
	public:
		vkWindow(int width, int height, std::string windowTitle, const vkEngine* pEngine);
		~vkWindow();
		void PollEvents(void);
		int WindowShouldClose();
		const char** GetRequiredInstanceExtensions(uint32_t* glfw_extension_count);
		bool CreateWin32Surface(VkSurfaceKHR* surface);
	private:
		GLFWwindow* m_pWindow;
		const vkEngine* m_pvkEngine;
	};
}