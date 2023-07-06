#include "vkWindow.h"
namespace vk
{
	vkWindow::vkWindow(int width, int height, std::string windowTitle)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_pWindow = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
		glfwShowWindow(m_pWindow);
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
}