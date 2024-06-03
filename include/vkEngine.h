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

		void Process(float deltaTime);
		void Render(float deltaTime);

		void CursorPosCallback(double xPos, double yPos);
		void MouseButtonCallback(int button, int action, int mods);
		void KeyCallback(int key, int scancode, int action, int mods);
		void FrameBufferSizeCallback(int width, int height);

		vkWindow* GetWindow();
		vkDevice* GetDevice() const;
		const vkPipelineManager* GetPipelineManager() const;
		const VkInstance& GetVulkanInstance() const;
		const VkSurfaceKHR& GetWindowSurface() const;
		const VkRect2D& GetWindowSize() const { return m_WindowSize; }

		vkGameObject* CreateQuad();

	private:
		static const vkEngine* m_pvkEngine;

		vkWindow* m_pWindow;
		VkRect2D m_WindowSize;
		VkSurfaceKHR m_WindowSurface;
		vkDevice* m_pvkDevice;
		VkInstance m_vkInstance;
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

		bool m_IsMouseDragging;

		bool InitializeWindow();
		bool InitializeVulkan();
		bool CreateWindowSurface();
		bool InitializeDevice();
		bool InitializePipelineManager();
		bool CreateFrameBuffers();
		bool CreateCommandBuffers();
		bool CreateSynchronizationPrimitives();
		bool CreateResourcePool();
	};
}