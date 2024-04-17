#include "vkWindow.h"
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_win32.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "vkEngine.h"
#include "vkCamera.h"
#include "vkScene.h"

namespace vk
{
	vkEngine* vkWindow::m_pStaticEngine = nullptr;
	vkWindow* vkWindow::m_pStaticWindow = nullptr;
	vkWindow::vkWindow(vkEngine* pEngine, const VkRect2D& windowSize, const std::string& windowTitle) 
		: m_pEngine(pEngine), m_WindowSize(windowSize), m_WindowTitle(windowTitle), m_WindowSurface(VK_NULL_HANDLE)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_pWindow = glfwCreateWindow(m_WindowSize.extent.width, m_WindowSize.extent.height, m_WindowTitle.c_str(), NULL, NULL);
		glfwShowWindow(m_pWindow);
		glfwSetWindowPos(m_pWindow, m_WindowSize.offset.x, m_WindowSize.offset.y);

		m_pStaticEngine = pEngine;
		m_pStaticWindow = this;

		glfwSetKeyCallback(m_pWindow, KeyCallback);
		glfwSetCursorPosCallback(m_pWindow, CursorPosCallback);
		glfwSetMouseButtonCallback(m_pWindow, MouseButtonCallback);
		glfwSetFramebufferSizeCallback(m_pWindow, FrameBufferSizeCallback);
		m_IsMouseDragging = false;
	}

	vkWindow::~vkWindow()
	{
		glfwDestroyWindow(m_pWindow);
		vkDestroySurfaceKHR(m_pEngine->GetVulkanInstance(), m_WindowSurface, nullptr); // dont destroy engine before this.
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

	const VkSurfaceKHR& vkWindow::CreateWin32Surface()
	{
		VkWin32SurfaceCreateInfoKHR createSurfaceInfo{};
		createSurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createSurfaceInfo.hwnd = glfwGetWin32Window(m_pWindow);
		createSurfaceInfo.hinstance = GetModuleHandle(nullptr);

		VkResult result = vkCreateWin32SurfaceKHR(m_pEngine->GetVulkanInstance(), &createSurfaceInfo, nullptr, &m_WindowSurface);
		if ( result != VK_SUCCESS) {
			vkLog->Log("vkWindow : Win32 Surface creation failed...");
		}
		return m_WindowSurface;
	}

	void vkWindow::CursorPosCallback(double xPos, double yPos)
	{
		if (m_IsMouseDragging) {
			// Calculate the cursor's delta movement
			float deltaX = static_cast<float>(xPos - m_LastX);
			float deltaY = static_cast<float>(yPos - m_LastY);

			// Update the last cursor position
			m_LastX = xPos;
			m_LastY = yPos;

			vkCamera* pCamera = m_pEngine->GetMainScene()->GetMainCamera();

			deltaX *= pCamera->GetMouseSensitivity();
			deltaY *= pCamera->GetMouseSensitivity();
			pCamera->UpdateCameraDelta(glm::vec3(0.0f, 0.0f, 0.0f), deltaX, deltaY);
		}


		m_pEngine->CursorPosCallback(xPos, yPos);
	}

	void vkWindow::MouseButtonCallback(int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				m_IsMouseDragging = true;
				glfwGetCursorPos(m_pWindow, &m_LastX, &m_LastY);
			}
			else if (action == GLFW_RELEASE) {
				m_IsMouseDragging = false;
			}
		}


		m_pEngine->MouseButtonCallback(button, action, mods);
	}

	void vkWindow::KeyCallback(int key, int scancode, int action, int mods)
	{
		vkCamera* pCamera = m_pEngine->GetMainScene()->GetMainCamera();
		switch (key)
		{
			case GLFW_KEY_PAGE_UP:
			{
				pCamera->UpdateCameraDelta(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
				break;
			}
			case GLFW_KEY_PAGE_DOWN:
			{
				pCamera->UpdateCameraDelta(glm::vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f);
				break;
			}
		}

		m_pEngine->KeyCallback(key, scancode, action, mods);

	}

	void vkWindow::FrameBufferSizeCallback(int width, int height)
	{
		m_pEngine->FrameBufferSizeCallback(width, height);
	}


	/// <summary>
	///  static functions
	/// </summary>

	void vkWindow::CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		m_pStaticWindow->CursorPosCallback(xPos, yPos);
	}

	void vkWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		m_pStaticWindow->MouseButtonCallback(button, action, mods);
	}

	void vkWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		m_pStaticWindow->KeyCallback(key, scancode, action, mods);
	}

	void vkWindow::FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		m_pStaticWindow->FrameBufferSizeCallback(width, height);
	}
}