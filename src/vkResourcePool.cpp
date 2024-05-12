#include "vkCore.h"
#include "vkResourcePool.h"
#include "vkMesh.h"
#include "vkGameObject.h"
namespace vk
{
	vkResourcePool* vkResourcePool::m_pInstance = nullptr;

	vkResourcePool* vkResourcePool::GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new vkResourcePool();
		}
		return m_pInstance;
	}

	vkResourcePool::vkResourcePool()
	{
	}

	vkResourcePool::~vkResourcePool()
	{
		for (std::vector<vkMesh*>::iterator itr = m_vMeshes.begin(); itr != m_vMeshes.end(); itr++)
		{
			delete (*itr);
		}
		m_vMeshes.clear();
	}

	uint32_t vkResourcePool::AddMesh(vkMesh* pMesh)
	{
		m_vMeshes.push_back(pMesh);
		return static_cast<uint32_t>(m_vMeshes.size()) - 1;
	}

	vkMesh* vkResourcePool::GetMesh(uint32_t index) const
	{
		if (index < static_cast<uint32_t>(m_vMeshes.size()))
		{
			return m_vMeshes[index];
		}
		return nullptr;
	}

	uint32_t vkResourcePool::GetNbMeshes() const
	{
		return static_cast<uint32_t>(m_vMeshes.size());
	}

	uint32_t vkResourcePool::AddMaterial(vkMaterial* pMaterial)
	{
		m_vMaterials.push_back(pMaterial);
		return static_cast<uint32_t>(m_vMaterials.size()) - 1;
	}

	vkMaterial* vkResourcePool::GetMaterial(uint32_t index) const
	{
		if (index < static_cast<uint32_t>(m_vMaterials.size()))
		{
			return m_vMaterials[index];
		}
		return nullptr;
	}

	uint32_t vkResourcePool::GetNbMaterials() const
	{
		return static_cast<uint32_t>(m_vMaterials.size());
	}


	uint32_t vkResourcePool::AddTexture(vkTexture* pTexture)
	{
		m_vTextures.push_back(pTexture);
		return static_cast<uint32_t>(m_vTextures.size()) - 1;
	}

	vkTexture* vkResourcePool::GetTexture(uint32_t index) const
	{
		if (index < static_cast<uint32_t>(m_vTextures.size()))
		{
			return m_vTextures[index];
		}
		return nullptr;
	}

	uint32_t vkResourcePool::GetNbTextures() const
	{
		return static_cast<uint32_t>(m_vTextures.size());
	}


	uint32_t vkResourcePool::AddGameObject(vkGameObject* pGameObject)
	{
		m_vGameObjects.push_back(pGameObject);
		return static_cast<uint32_t>(m_vGameObjects.size()) - 1;
	}

	vkGameObject* vkResourcePool::GetGameObject(uint32_t index) const
	{
		if (index < static_cast<uint32_t>(m_vGameObjects.size()))
		{
			return m_vGameObjects[index];
		}
		return nullptr;
	}

	uint32_t vkResourcePool::GetNbGameObjects() const
	{
		return static_cast<uint32_t>(m_vGameObjects.size());
	}
}