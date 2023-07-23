#include "vkWindow.h"
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_win32.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "vkEngine.h"


namespace vk
{
	vkWindow::vkWindow(int width, int height, std::string windowTitle, const vkEngine* pEngine)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_pWindow = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
		glfwShowWindow(m_pWindow);
		m_pvkEngine = pEngine;
	}

	vkWindow::~vkWindow()
	{
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}
	void vkWindow::PollEvents(void)
	{
		glfwPollEvents();
	}
	int vkWindow::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_pWindow);
	}

	const char** vkWindow::GetRequiredInstanceExtensions(uint32_t* glfw_extension_count)
	{
		return glfwGetRequiredInstanceExtensions(glfw_extension_count);
	}

	bool vkWindow::CreateWin32Surface(VkSurfaceKHR* surface)
	{
		VkWin32SurfaceCreateInfoKHR createSurfaceInfo{};
		createSurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createSurfaceInfo.hwnd = glfwGetWin32Window(m_pWindow);
		createSurfaceInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(m_pvkEngine->GetInstance(), &createSurfaceInfo, nullptr, surface) != VK_SUCCESS)
		{
			return false;
		}
		return true;
	}
}