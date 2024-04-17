#include "vkCore.h"
#include "vkEngine.h"
#include <windows.h>
#include "vkLogger.h"
#include "vkWindow.h"
#include "vkDevice.h"
#include "vkPipelineManager.h"
#include <vector>
namespace vk
{
	vkEngine::vkEngine()
	{
		m_pvkWindow = nullptr;
		m_pvkDevice = nullptr;
		m_pvkPipelineManager = nullptr;
		if (InitializeWindow() == false)
		{
			vkLog->Log("Window initialization failed...");
			return;
		}
		
		if (InitializeVulkan() == false)
		{
			vkLog->Log("Vulkan initialization failed...");
			return;
		}

		if (CreateSurface() == false)
		{
			vkLog->Log("Creation Win32 Surface Failed");
			return;
		}

		if (InitializeDevice() == false)
		{
			vkLog->Log("Device initialization failed...");
			return;
		}

		if (InitializePipelineManager() == false)
		{
			vkLog->Log("PipelineManager initialization failed...");
		}

		if (CreateFrameBuffers() == false)
		{
			vkLog->Log("Frame Buffers creation failed...");
		}

		if (CreateCommandBuffers() == false)
		{
			vkLog->Log("Command Buffers creation failed...");
		}

		if (CreateSynchronizationPrimitives() == false)
		{
			vkLog->Log("SynchronizationPrimitives creation failed...");
		}

		m_bEngineRunning = false;
	}

	vkEngine::~vkEngine()
	{
		vkDestroyInstance(m_vkInstance, nullptr);
		delete m_pvkWindow;

		delete m_pvkPipelineManager;
		delete m_pvkDevice;

		vkDestroyFence(m_pvkDevice->GetLogicalDevice(), m_vkWaitFence, NULL);
		vkDestroySemaphore(m_pvkDevice->GetLogicalDevice(), m_vkRenderCompletedSemaphore, NULL);
		vkDestroySemaphore(m_pvkDevice->GetLogicalDevice(), m_vkPresentCompletedSemaphore, NULL);
	}

	vkWindow* vkEngine::GetWindow()
	{
		return m_pvkWindow;
	}
	vkDevice* vkEngine::GetDevice()
	{
		return m_pvkDevice;
	}

	vkPipelineManager* vkEngine::GetPipelineManager()
	{
		return m_pvkPipelineManager;
	}

	const VkInstance& vkEngine::GetInstance() const
	{
		return m_vkInstance;
	}
	const VkSurfaceKHR& vkEngine::GetSurface() const
	{
		return m_vkSurface;
	}

	bool vkEngine::InitializeWindow()
	{
		m_pvkWindow = new vkWindow(1600, 900, std::string("Vulkan Engine"), this);
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

		vkLog->Log("Supported instance level extension properties :");
		for (const auto& extension : extensions) {
			vkLog->Log("	", extension.extensionName);
		}
		return true;
	}

	bool vkEngine::CreateSurface()
	{
		return m_pvkWindow->CreateWin32Surface(&m_vkSurface);
	}

	bool vkEngine::InitializeDevice()
	{
		m_pvkDevice = new vkDevice(m_vkInstance, this);
		return true;
	}

	bool vkEngine::InitializePipelineManager()
	{
		m_pvkPipelineManager = new vkPipelineManager(this);
		return true;
	}

	bool vkEngine::CreateFrameBuffers()
	{
		return m_pvkDevice->CreateFrameBuffers();
	}

	bool vkEngine::CreateCommandBuffers()
	{
		return m_pvkDevice->CreateCommandBuffers();
	}

	bool vkEngine::CreateSynchronizationPrimitives()
	{
		bool bStatus = true;
		// Create a fence
		VkFenceCreateInfo fenceCI = vk::initializers::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		
		VkResult result = vkCreateFence(m_pvkDevice->GetLogicalDevice(), &fenceCI, NULL, &m_vkWaitFence);

		if (result != VK_SUCCESS) {
			vkLog->Log("Fence is not created...");
			bStatus = false;
		}
		vkLog->Log("Fences created...");

		// Create a semaphore
		VkSemaphoreCreateInfo semaphoreCI = vk::initializers::SemaphoreCreateInfo();
		result = vkCreateSemaphore(m_pvkDevice->GetLogicalDevice(), &semaphoreCI, NULL, &m_vkPresentCompletedSemaphore);
		if (result != VK_SUCCESS) {
			vkLog->Log("Semaphore is not created...");
			bStatus = false;
		}

		result = vkCreateSemaphore(m_pvkDevice->GetLogicalDevice(), &semaphoreCI, NULL, &m_vkRenderCompletedSemaphore);
		if (result != VK_SUCCESS) {
			vkLog->Log("Semaphore is not created...");
			bStatus = false;
		}

		vkLog->Log("Semaphores created...");

		return bStatus;
	}

	void vkEngine::Process(float fTimeElapsed)
	{

	}
	void vkEngine::Render(float fTimeElapsed)
	{
		const VkDevice vkDevice = m_pvkDevice->GetLogicalDevice();
		const VkSwapchainKHR swapChain = m_pvkDevice->GetSwapChain();
		const VkCommandBuffer commandBuffer = m_pvkDevice->GetCommandBuffers();
		const std::vector<VkFramebuffer> vFrameBuffers = m_pvkDevice->GetFrameBuffers();
		const VkExtent2D swapChainExtent = m_pvkDevice->GetSwapChainExtent();
		const VkRenderPass renderPass = m_pvkPipelineManager->GetRenderPass(vk::eRenderPass::RP_Forward_Rendering_Geometry);
		
		VkResult result = vkWaitForFences(vkDevice, 1, &m_vkWaitFence, VK_TRUE, UINT64_MAX);
		// Reset fences
		result = vkResetFences(vkDevice, 1, &m_vkWaitFence);

		uint32_t imageIndex;
		result = vkAcquireNextImageKHR(vkDevice, swapChain, UINT64_MAX, m_vkPresentCompletedSemaphore, VK_NULL_HANDLE, &imageIndex);


		// Record command buffer with the current image as attachment in the framebuffer
		vkResetCommandBuffer(commandBuffer, 0);
		//recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
		//
		// 
		// 
		VkCommandBufferBeginInfo cmdBufInfo{};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		// Set clear values for all framebuffer attachments with loadOp set to clear
		// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = swapChainExtent.width;
		renderPassBeginInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		renderPassBeginInfo.framebuffer = vFrameBuffers[imageIndex];

		//const VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
		vkBeginCommandBuffer(commandBuffer, &cmdBufInfo);

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdEndRenderPass(commandBuffer);

		vkEndCommandBuffer(commandBuffer);
		// 
		// ////////////////////////////////////////////////////////////////
		
		
		// Submit the command buffer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_vkPresentCompletedSemaphore };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore renderCompletedSemaphores[] = { m_vkRenderCompletedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = renderCompletedSemaphores;

		result = vkQueueSubmit(m_pvkDevice->GetGraphicsQueue(), 1, &submitInfo, m_vkWaitFence);

		// Present the image to the swap chain
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = renderCompletedSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(m_pvkDevice->GetPresentQueue(), &presentInfo);
		if (result != VK_SUCCESS){
			vkLog->Log("Presenting to swapchain failed..");
		}
	}

	bool vkEngine::StartEngine()
	{
		m_bEngineRunning = true;

		auto startTime = std::chrono::high_resolution_clock::now();
		while (m_bEngineRunning)
		{
			if (m_pvkWindow->WindowShouldClose())
			{
				m_bEngineRunning = false;
				break;
			}

			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
			startTime = endTime;
			float timeElapsed = (float)elapsed.count() / 1000.0f;  // converts into seconds
			Process(timeElapsed);
			Render(timeElapsed);
				
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
