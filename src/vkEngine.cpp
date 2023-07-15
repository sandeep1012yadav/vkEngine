#include "vkEngine.h"
#include <windows.h>
#include "vkLogger.h"

#include "vkWindow.h"
#include "vkDevice.h"
#include <vector>
namespace vk
{
	vkEngine::vkEngine()
	{
		m_pvkWindow = nullptr;
		m_pvkDevice = nullptr;
		if (InitializeWindow() == false)
		{
			vkLog->Log("Window Initialization Failed");
		}
		
		if (InitializeVulkan() == false)
		{
			vkLog->Log("Vulkan Initialization Failed");
		}

		if (InitializeDevice() == false)
		{
			vkLog->Log("Device Initialization Failed");
		}

		m_bEngineRunning = false;
	}

	vkEngine::~vkEngine()
	{
		vkDestroyInstance(m_vkInstance, nullptr);
		delete m_pvkWindow;
	}

	bool vkEngine::InitializeWindow()
	{
		m_pvkWindow = new vkWindow(1600, 900, std::string("Vulkan Engine"));
		return true;
	}

	bool vkEngine::InitializeVulkan()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "VulkanTest";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "vkEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = m_pvkWindow->GetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;
		if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
		{
			return false;
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
			nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
			extensions.data());
		for (const auto& extension : extensions) {
			vkLog->Log(extension.extensionName);
		}
		return true;
	}

	bool vkEngine::InitializeDevice()
	{
		m_pvkDevice = new vkDevice(m_vkInstance);
		return true;
	}

	bool vkEngine::StartEngine()
	{
		m_bEngineRunning = true;
		while (m_bEngineRunning)
		{
			if (m_pvkWindow->WindowShouldClose())
			{
				m_bEngineRunning = false;
				break;
			}
				
			m_pvkWindow->PollEvents();
			Sleep(20);
		}
		return true;
	}

	bool vkEngine::StopEngine()
	{
		m_bEngineRunning = false;
		return true;
	}
}
