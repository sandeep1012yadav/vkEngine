#pragma once

#include "vkCore.h"
#include "vkLogger.h"
namespace vk
{
	class vkWindow;
	class vkDevice;
	class vkPipelineManager;
	class vkResourcePool;
	class vkGameObject;
	class vkScene;
	class vkRenderer;
	class vkEngine
	{
	public:
		vkEngine();
		~vkEngine();

		static const vkEngine* GetInstance() { return m_pvkEngine; }
		static void SetInstance(const vkEngine* pEngine) { m_pvkEngine = pEngine; }

		vkScene* CreateScene();
		vkScene* CreateScene(const std::string& sceneName);
		uint32_t AddScene(vkScene* pScene, bool isMainScene);
		vkScene* GetScene(uint32_t sceneId) const;
		vkScene* GetMainScene() const;
		void SetMainScene(uint32_t sceneId);
		bool StartEngine();
		bool StopEngine();

		void Process(float fTimeElapsed);
		void Render(float fTimeElapsed);

		vkWindow* GetWindow();
		vkDevice* GetDevice() const;
		vkPipelineManager* GetPipelineManager();
		const VkInstance& GetVulkanInstance() const;
		const VkSurfaceKHR& GetSurface() const;

		void AddQuadToScene();

	private:
		static const vkEngine* m_pvkEngine;

		vkWindow* m_pvkWindow;
		vkDevice* m_pvkDevice;
		VkInstance m_vkInstance;
		VkSurfaceKHR m_vkSurface;
		bool m_bEngineRunning;
		vkPipelineManager* m_pvkPipelineManager;
		vkResourcePool* m_pResourcePool;
		vkRenderer* m_pRenderer;

		VkFence m_vkWaitFence;
		VkSemaphore m_vkPresentCompletedSemaphore;
		VkSemaphore m_vkRenderCompletedSemaphore;

		std::vector<vkScene*> m_vScenes;
		vkScene* m_pMainScene;
		uint32_t m_MainSceneId;

		bool InitializeWindow();
		bool InitializeVulkan();
		bool CreateSurface();
		bool InitializeDevice();
		bool InitializePipelineManager();
		bool CreateFrameBuffers();
		bool CreateCommandBuffers();
		bool CreateSynchronizationPrimitives();
		bool CreateResourcePool();
	};
}