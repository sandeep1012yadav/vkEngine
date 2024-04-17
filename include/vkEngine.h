#pragma once

#include "vkCore.h"
#include "vkLogger.h"
namespace vk
{
	class vkWindow;
	class vkDevice;
	class vkPipelineManager;
	class vkEngine
	{
	public:
		vkEngine();
		
		bool StartEngine();
		bool StopEngine();

		void Process(float fTimeElapsed);
		void Render(float fTimeElapsed);

		vkWindow* GetWindow();
		vkDevice* GetDevice();
		vkPipelineManager* GetPipelineManager();
		const VkInstance& GetInstance() const;
		const VkSurfaceKHR& GetSurface() const;

		~vkEngine();

	private:
		vkWindow* m_pvkWindow;
		vkDevice* m_pvkDevice;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_vkSurface;
		bool m_bEngineRunning;
		vkPipelineManager* m_pvkPipelineManager;

		VkFence m_vkWaitFence;
		VkSemaphore m_vkPresentCompletedSemaphore;
		VkSemaphore m_vkRenderCompletedSemaphore;

		bool InitializeWindow();
		bool InitializeVulkan();
		bool CreateSurface();
		bool InitializeDevice();
		bool InitializePipelineManager();
		bool CreateFrameBuffers();
		bool CreateCommandBuffers();
		bool CreateSynchronizationPrimitives();
	};
}