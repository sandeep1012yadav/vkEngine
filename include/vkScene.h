#pragma once
#include "vkCore.h"
#include "vkObject.h"
namespace vk
{
	class vkEngine;
	class vkGameObject;
	class vkCamera;
	class vkScene : public vkObject
	{
	public:
		uint32_t AddGameObject(vkGameObject* pGameObject);  // return gameobject id
		uint32_t AddCamera(vkCamera* pCamera); // return camera id;
		void SetMainCamera(uint32_t cameraId);

		vkCamera* GetCamera(uint32_t cameraId) const;
		vkCamera* GetMainCamera() const;
		vkGameObject* GetGameObject(uint32_t objId) const;

		const std::vector<vkGameObject*>& GetSceneGameObjects() const;

		uint32_t GetNbSceneUniformBuffers() const;
		uint32_t GetNbFrameUniformBuffers() const;


		void Preprocess();
		

	private:
		vkScene(const vkEngine* pEngine);
		vkScene(const std::string& name, const vkEngine* pEngine);
		
		const vkEngine* m_pEngine;
		std::vector<vkGameObject*> m_vSceneGameObjects;
		std::vector<vkCamera*> m_vSceneCameras;

		vkCamera* m_pMainCamera;
		uint32_t m_MainCameraId;

		uint32_t m_NbSceneUniformBuffers;
		uint32_t m_NbFrameUniformBuffers;

		friend class vkEngine;
	};

	
}