#include "vkCore.h"
#include "vkResourcePool.h"
#include "vkMesh.h"
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

	vkMesh* vkResourcePool::GetMesh(uint32_t index)
	{
		if (index < static_cast<uint32_t>(m_vMeshes.size()))
		{
			return m_vMeshes[index];
		}
		return nullptr;
	}

	uint32_t vkResourcePool::GetNbMeshes()
	{
		return static_cast<uint32_t>(m_vMeshes.size());
	}
}