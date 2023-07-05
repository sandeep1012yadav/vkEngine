#pragma once
#include "GLFW/glfw3.h"
#include <string>
namespace vk
{
	class vkWindow
	{
	public:
		vkWindow(int width, int height, std::string windowTitle);
		~vkWindow();
		void PollEvents(void);
		int WindowShouldClose();
		const char** GetRequiredInstanceExtensions(uint32_t* glfw_extension_count);
	private:
		GLFWwindow* m_pWindow;
	};
}