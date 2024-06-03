#pragma once
#include "vkCore.h"
#include "GLFW/glfw3.h"

namespace vk
{
	class vkEngine;
	class vkWindow
	{
	public:
		vkWindow(vkEngine* pEngine, const VkRect2D& windowSize , const std::string& windowTitle);
		~vkWindow();
		void PollEvents(void);
		int WindowShouldClose();
		const char** GetRequiredInstanceExtensions(uint32_t* glfw_extension_count);
		
		const VkRect2D& GetWindowSize() const { return m_WindowSize; }
		const VkSurfaceKHR& GetWindowSurface() const { return m_WindowSurface; }
		const VkSurfaceKHR& CreateWin32Surface();


		void CursorPosCallback(double xPos, double yPos);
		void MouseButtonCallback(int button, int action, int mods);
		void KeyCallback(int key, int scancode, int action, int mods);
		void FrameBufferSizeCallback(int width, int height);
		void ScrollCallback(double xOffset, double yOffset);


		static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
		static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	private:
		static vkEngine* m_pStaticEngine;
		static vkWindow* m_pStaticWindow;
		GLFWwindow* m_pWindow;
		vkEngine* m_pEngine;
		std::string m_WindowTitle;
		VkRect2D m_WindowSize;
		VkSurfaceKHR m_WindowSurface;
		
		//Camera related setting
		bool m_IsMouseDragging;
		//double m_LastX, m_LastY;
		glm::vec2 mCurrentPos, mLastPos;
	};
}