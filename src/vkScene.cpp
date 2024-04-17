#include "vkScene.h"
#include "vkCamera.h"
#include "vkGameObject.h"
#include "vkFrameObject.h"

namespace vk
{
	vkScene::vkScene(const vkEngine* pEngine) : vkObject("noName", ObjectType::OT_Scene), m_pEngine(pEngine)
	{
		m_pMainCamera = new vkCamera("SceneMainCamera");
		m_MainCameraId = 0;
		m_vSceneCameras.push_back(m_pMainCamera);
		m_NbFrameUniformBuffers = 0;
		m_NbSceneUniformBuffers = 1;
	}

	vkScene::vkScene(const std::string& name, const vkEngine* pEngine) : vkObject(name, ObjectType::OT_Scene), m_pEngine(pEngine)
	{
		m_pMainCamera = new vkCamera("SceneMainCamera");
		m_MainCameraId = 0;
		m_vSceneCameras.push_back(m_pMainCamera);
		m_NbFrameUniformBuffers = 0;
		m_NbSceneUniformBuffers = 1;
	}

	uint32_t vkScene::AddGameObject(vkGameObject* pGameObject)
	{
		uint32_t objIndex = static_cast<uint32_t>(m_vSceneGameObjects.size());
		m_vSceneGameObjects.push_back(pGameObject);
		return objIndex;
	}

	uint32_t vkScene::AddCamera(vkCamera* pCamera)
	{
		uint32_t camIndex = static_cast<uint32_t>(m_vSceneCameras.size());
		m_vSceneCameras.push_back(pCamera);
		return camIndex;
	}

	void vkScene::SetMainCamera(uint32_t cameraId)
	{
		m_MainCameraId = cameraId;
		m_pMainCamera = m_vSceneCameras[m_MainCameraId];
	}

	vkCamera* vkScene::GetCamera(uint32_t cameraId) const
	{
		return m_vSceneCameras[cameraId];
	}

	vkCamera* vkScene::GetMainCamera() const
	{
		return m_vSceneCameras[m_MainCameraId];
	}

	vkGameObject* vkScene::GetGameObject(uint32_t objId) const
	{
		return m_vSceneGameObjects[objId];
	}

	const std::vector<vkGameObject*>& vkScene::GetSceneGameObjects() const
	{
		return m_vSceneGameObjects;
	}

	uint32_t vkScene::GetNbSceneUniformBuffers() const
	{
		return m_NbSceneUniformBuffers;
	}

	uint32_t vkScene::GetNbFrameUniformBuffers() const
	{
		return m_NbFrameUniformBuffers;
	}

	void vkScene::Preprocess()
	{
		m_NbSceneUniformBuffers = 1;
		m_NbFrameUniformBuffers = 0;
		for (auto it = m_vSceneGameObjects.begin(); it != m_vSceneGameObjects.end(); it++)
		{
			vkGameObject* obj = *it;
			obj->Preprocess();
			m_NbFrameUniformBuffers += obj->mNbUniformBuffers;
		}
	}
}