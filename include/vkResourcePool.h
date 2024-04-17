#pragma once
#include "vkCore.h"
namespace vk
{
	class vkMesh;
	class vkEngine;
	class vkResourcePool
	{
	public:
		static vkResourcePool* GetInstance();
		uint32_t AddMesh(vkMesh* pMesh);
		vkMesh* GetMesh(uint32_t index);
		uint32_t GetNbMeshes();

	private:
		static vkResourcePool* m_pInstance;
		vkResourcePool();
		~vkResourcePool();

		std::vector<vkMesh*> m_vMeshes;
	};

	

	
}