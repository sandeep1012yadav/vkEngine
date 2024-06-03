#include "vkCore.h"
#include <windows.h>
#include "vkLogger.h"
#include "vkWindow.h"
#include "vkDevice.h"
#include "vkPipelineManager.h"
#include "vkResourcePool.h"
#include "vkMesh.h"
#include "vkFrameObject.h"
#include "vkGameObject.h"
#include "vkScene.h"
#include "vkRenderer.h"
#include "vkResourceLoader.h"
#include "vkSamplerManager.h"
#include "vkMaterial.h"
#include "vkEngine.h"
namespace vk
{
	const vkEngine* vkEngine::m_pvkEngine = nullptr;

	vkEngine::vkEngine()
	{
		std::string s = "\
						*******************************************************\n\
						****************Welcome To Vulkan Engine***************\n\
						*******************************************************\n\
						*******************************************************";
		vk::vkLog->Log(s);

		m_pWindow = nullptr;
		m_pvkDevice = nullptr;
		m_pvkPipelineManager = nullptr;
		m_pRenderer = nullptr;
		m_IsMouseDragging = false;
		
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

		if (CreateWindowSurface() == false)
		{
			vkLog->Log("Window surface creation failed...");
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

		if (CreateResourcePool() == false)
		{
			vkLog->Log("Resource Pool creation failed...");
		}

		m_bEngineRunning = false;

		SetInstance(this);

		vkResourceLoader::CreateInstance(this);

		vkSamplerManager::CreateInstance(this);

	}

	vkEngine::~vkEngine()
	{
		delete m_pWindow;
		delete m_pvkPipelineManager;
		delete m_pvkDevice;
		
		vkDestroyFence(m_pvkDevice->GetLogicalDevice(), m_vkWaitFence, NULL);
		vkDestroySemaphore(m_pvkDevice->GetLogicalDevice(), m_vkRenderCompletedSemaphore, NULL);
		vkDestroySemaphore(m_pvkDevice->GetLogicalDevice(), m_vkPresentCompletedSemaphore, NULL);
		vkDestroyInstance(m_vkInstance, nullptr);

		vkSamplerManager::DestroyInstance();
	}

	vkWindow* vkEngine::GetWindow()
	{
		return m_pWindow;
	}

	vkDevice* vkEngine::GetDevice() const
	{
		return m_pvkDevice;
	}

	const vkPipelineManager* vkEngine::GetPipelineManager() const
	{
		return m_pvkPipelineManager;
	}

	const VkInstance& vkEngine::GetVulkanInstance() const
	{
		return m_vkInstance;
	}
	const VkSurfaceKHR& vkEngine::GetWindowSurface() const
	{
		return m_WindowSurface;
	}

	bool vkEngine::InitializeWindow()
	{
		m_WindowSize.offset = { 50, 50 };
		m_WindowSize.extent = { 1600, 900 };
		m_pWindow = new vkWindow(this, m_WindowSize, std::string("Vulkan Engine"));
		
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
		glfwExtensions = m_pWindow->GetRequiredInstanceExtensions(&glfwExtensionCount);

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

	bool vkEngine::CreateWindowSurface()
	{
		m_WindowSurface = m_pWindow->CreateWin32Surface();
		return true;
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

	bool vkEngine::CreateResourcePool()
	{
		bool bStatus = true;
		m_pResourcePool = vkResourcePool::GetInstance();
		vkLog->Log("Resource Pool created...");
		return bStatus;
	}

	vkScene* vkEngine::CreateScene()
	{
		vkScene* pScene = new vkScene("noName", this);
		return pScene;
	}

	vkScene* vkEngine::CreateScene(const std::string& sceneName)
	{
		vkScene* pScene = new vkScene(sceneName, this);
		return pScene;
	}

	uint32_t vkEngine::AddScene(vkScene* pScene, bool isMainScene)
	{
		uint32_t sceneId = static_cast<uint32_t>(m_vScenes.size());
		m_vScenes.push_back(pScene);
		if (isMainScene)
		{
			m_MainSceneId = sceneId;
			m_pMainScene = pScene;
		}
		return sceneId;
	}

	vkScene* vkEngine::GetScene(uint32_t sceneId) const
	{
		return m_vScenes[sceneId];
	}

	vkScene* vkEngine::GetMainScene() const
	{
		return m_pMainScene;
	}

	void vkEngine::SetMainScene(uint32_t sceneId)
	{
		m_MainSceneId = sceneId;
		m_pMainScene = m_vScenes[m_MainSceneId];
	}

	void vkEngine::Process(float deltaTime)
	{

	}
	void vkEngine::Render(float deltaTime)
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

		VkCommandBufferBeginInfo cmdBufInfo = vk::initializers::CommandBufferBeginInfo(0);

		// Set clear values for all framebuffer attachments with loadOp set to clear
		// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.2f, 0.2f, 0.5f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRect2D renderArea = { {0, 0}, {swapChainExtent.width, swapChainExtent.height} };
		VkRenderPassBeginInfo renderPassBeginInfo =  vk::initializers::RenderPassBeginInfo(renderPass, renderArea, 2, clearValues, vFrameBuffers[imageIndex]);

		//const VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
		vkBeginCommandBuffer(commandBuffer, &cmdBufInfo);

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


		VkViewport viewport{};
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		// Update dynamic scissor state
		VkRect2D scissor{};
		scissor.extent.width = swapChainExtent.width;
		scissor.extent.height = swapChainExtent.height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		m_pRenderer->RenderScene(commandBuffer, deltaTime);


		vkCmdEndRenderPass(commandBuffer);

		vkEndCommandBuffer(commandBuffer);
		// 
		// ////////////////////////////////////////////////////////////////
		
		
		// Submit the command buffer
		std::vector<VkCommandBuffer> vCommandBuffers = { commandBuffer };
		std::vector<VkSemaphore> vWaitSemaphores = { m_vkPresentCompletedSemaphore };
		std::vector<VkPipelineStageFlags> vWaitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		std::vector<VkSemaphore> vSignalSemaphores = { m_vkRenderCompletedSemaphore };
		
		VkSubmitInfo submitInfo = vk::initializers::SubmitInfo(vCommandBuffers, vWaitSemaphores, vWaitStages, vSignalSemaphores);

		result = vkQueueSubmit(m_pvkDevice->GetGraphicsQueue(), 1, &submitInfo, m_vkWaitFence);

		// Present the image to the swap chain
		std::vector<VkSwapchainKHR> vSwapchains = { swapChain };
		VkPresentInfoKHR presentInfo = vk::initializers::PresentInfoKHR(imageIndex, vSignalSemaphores, vSwapchains);
		result = vkQueuePresentKHR(m_pvkDevice->GetPresentQueue(), &presentInfo);
		if (result != VK_SUCCESS){
			vkLog->Log("Presenting to swapchain failed..");
		}
	}

	vkGameObject* vkEngine::CreateQuad()
	{
		vkVertex v1 = {
			{-50.0f, 0.0f, -50.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f}
		};

		vkVertex v2 = {
			{50.0f, 0.0f, -50.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f}
		};

		vkVertex v3 = {
			{50.0f, 0.0f, 50.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f}
		};

		vkVertex v4 = {
			{-50.0f, 0.0f, 50.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f}
		};

		std::vector<vkVertex> quadVertices = { v1, v2, v3, v4 };
		std::vector<uint32_t> quadIndices = { 0, 1, 2, 0, 3, 2 };

		vkGameObject* pGameObject = new vkGameObject("QuadGameObject");
		pGameObject->m_vVertices = std::move(quadVertices);
		pGameObject->m_vIndices = std::move(quadIndices);

		pGameObject->m_pFrameObject = new vkFrameObject("QuadFrameObject");
		
		vkMesh*& pMesh = pGameObject->m_pFrameObject->m_pMesh;

		vkMaterial* pMaterial = new vkMaterial(this, "QuadMaterial");
		uint32_t matIndex = vkResourcePool::GetInstance()->AddMaterial(pMaterial);
		
		pMesh = new vkMesh("QuadMesh");
		
		vkPrimitive primitive = { 0, 6, matIndex };
		pMesh->m_vPrimitives.push_back(primitive);

		return pGameObject;
	}

	bool vkEngine::StartEngine()
	{
		//Preprocessing phase/////
		m_pRenderer = new vkRenderer(this, m_pvkPipelineManager);
		//////////////////////////

		m_bEngineRunning = true;

		auto startTime = std::chrono::high_resolution_clock::now();
		while (m_bEngineRunning)
		{
			if (m_pWindow->WindowShouldClose())
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
				
			m_pWindow->PollEvents();
			Sleep(20);
		}
		return true;
	}

	bool vkEngine::StopEngine()
	{
		m_bEngineRunning = false;
		return true;
	}

	void vkEngine::CursorPosCallback(double xPos, double yPos)
	{
	}

	void vkEngine::MouseButtonCallback(int button, int action, int mods)
	{
	}

	void vkEngine::KeyCallback(int key, int scancode, int action, int mods)
	{
	}

	void vkEngine::FrameBufferSizeCallback(int width, int height)
	{
	}
}
