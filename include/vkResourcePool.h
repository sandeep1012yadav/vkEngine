#pragma once
#include "vkCore.h"
namespace vk
{
	class vkMesh;
	class vkTexture;
	class vkMaterial;
	class vkGameObject;
	class vkEngine;
	class vkResourcePool
	{
	public:
		static vkResourcePool* GetInstance();
		uint32_t AddMesh(vkMesh* pMesh);
		vkMesh* GetMesh(uint32_t index) const;
		uint32_t GetNbMeshes() const;

		uint32_t AddMaterial(vkMaterial* pMaterial);
		vkMaterial* GetMaterial(uint32_t index) const;
		uint32_t GetNbMaterials() const;

		uint32_t AddTexture(vkTexture* pTexture);
		vkTexture* GetTexture(uint32_t index) const;
		uint32_t GetNbTextures() const;

		uint32_t AddGameObject(vkGameObject* pGameObject);
		vkGameObject* GetGameObject(uint32_t index) const;
		uint32_t GetNbGameObjects() const;

	private:
		static vkResourcePool* m_pInstance;
		vkResourcePool();
		~vkResourcePool();

		std::vector<vkMesh*> m_vMeshes;
		std::vector<vkMaterial*> m_vMaterials;
		std::vector<vkTexture*> m_vTextures;
		std::vector<vkGameObject*> m_vGameObjects;
	};

	

	
}